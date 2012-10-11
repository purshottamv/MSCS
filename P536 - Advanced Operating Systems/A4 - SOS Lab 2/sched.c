#include "simulator.h"

#include "jval.h"
#include "dllist.h"
#include "mt.h"
#include "process.h"
#include "jrb.h"
#include <errno.h>

void schedule(){ 
    Jval val;
    Dllist dl;
    mt_joinall(); 
    if (dll_empty(readyq)) {
        DEBUG('e', "running noop....\n");
        noop();    
    } else {
        dl = dll_prev(readyq);
        val = dll_val(dl);
        dll_delete_node(dl);
        PCB *p = (PCB *)jval_v(val); 
        cp = p;        
        p->save_state = SAVE_NONE;
        User_Base = p->base;
        User_Limit = p->limit;
        DEBUG('e', "running user code size %d base: %d limit: %d\n", MemorySize, p->base, p->limit);
        run_user_code(p->rgs);                        
    }    
}

PCB * new_pcb() {
    int i = 0;
    PCB *p = (PCB *)malloc(sizeof(PCB));
    for (i = 0; i < NumTotalRegs; i++) {
        p->rgs[i] = 0;
    }    
    p->buf = NULL;
    p->buf_size = 0;
    p->chars_written = 0;
   
    p->rbuf = NULL;
    p->rbuf_chars = 0;
    p->rbuf_size = 0;
   
    p->sbrk = 0;
    p->base = 0;
    p->limit = 0;
 
    p->pid = 0;
    p->mem_partition = 0; 
   
    p->parent = NULL;
    p->waiter_sem = mt_sem_create(0);
    p->waiters = new_dllist();

	// initialize the children rb tree
	p->children = make_jrb();		// Step 21
    return p;
}

int perform_execve(PCB *p, char *fn, char **argv) {
    char *name = NULL;
    int argc = 0, i = 0;
    int loc = 64, l;
    int base = p->base;
    int end = p->limit;

    if (argv != NULL) {
        while (argv[i] != NULL) {
            if (i == 0) name = argv[i];
            i++;        
        }   
        argc = i;
    }
    int sbrk = 0;
    DEBUG('e', "trying to load the program %s\n",name);
    if ((sbrk = load_user_program(name)) < 0) {
        return -1;
    }
    int locations[256], tempLoc = 0;
    /** Align to a 4byte word */
    for (i = 0; i < argc; i++) {
        tempLoc += strlen(argv[i]) + 1;
        //locations[i] = loc;
    }
    loc += (4 - tempLoc % 4);  
    for (i = 0; i < argc; i++) {
        loc += strlen(argv[i]) + 1;
        memcpy(main_memory + p->base + end - loc,
           argv[i], strlen(argv[i]) + 1);
        locations[i] = loc;
        DEBUG('e', "locations %d %d %s\n", i, end - locations[i], main_memory + p->base + end - locations[i]);
    }   
   
    loc += sizeof(char *) * (argc + 1);
    for (i = 0; i < argc; i++) {
        l = (int)(end - locations[i]);  
        char *c = main_memory + p->base + end - loc + i * sizeof(char *);
        memcpy(c, &l, 4);
    }
    l = 0;     
    memcpy(main_memory + p->base + end - loc + argc * sizeof(char *), &l, 4);
    for (i = 0; i <= argc; i++) {
        DEBUG('e', "pointers %d %d pointing %d\n", i, end - loc + i * sizeof(char *), *((int *)(main_memory + p->base + end - loc + i * sizeof(char *))));
    }
    
    p->sbrk = sbrk;
    p->rgs[PCReg] = 0;
    p->rgs[NextPCReg] = 4;
    p->rgs[StackReg] = end - loc - 40;
    DEBUG('e', "stack %d\n", end - loc - 40);
    l = (int)(end - loc);  
    DEBUG('e', "loc %d\n", l);
    memcpy(main_memory + p->base + p->rgs[StackReg] + 16, &l, 4); 
    memcpy(main_memory + p->base + p->rgs[StackReg] + 12, &argc, 4);
    return 0;
}

void initialize_user_process(void *args) {
    char **argv = (char **)args;
    char *name = NULL;
    int argc = 0, i = 0;

    if (argv != NULL) {
        while (argv[i] != NULL) {
            if (i == 0) name = argv[i];
            i++;        
        }   
        argc = i;
    }

    int partition = get_partition();
    User_Base = partitions[partition].base;
    User_Limit =  partitions[partition].limit; 
   
    DEBUG('e', "Initialization with %d base %d limit %d\n", i, partitions[partition].base, partitions[partition].limit); 
    PCB *p = new_pcb();
    p->rgs[PCReg] = 0;
    p->rgs[NextPCReg] = 4;
    p->base = partitions[partition].base;
    p->limit = partitions[partition].limit;
    p->pid = get_new_pid();
    p->mem_partition = partition;
    p->parent = init;

    if (!perform_execve(p, name, argv)) {
        Jval val = new_jval_v(p);
        dll_prepend(readyq, val);
        mt_exit();
    } else {
        exit(1);
    }
}

void save_current_process() {
    if (cp != NULL && cp->save_state == SAVE_NONE) {
        examine_registers(cp->rgs);
        cp->save_state = SAVE;
        Jval val = new_jval_v(cp);
        dll_prepend(readyq, val);
        cp->save_state = SAVE_QUEUED;
        cp = NULL;
    }
}

void save_current_process_with_rgs(int rgs[]) {
    if (cp != NULL && cp->save_state == SAVE_NONE) {
        int i = 0;
        for (i = 0; i < NumTotalRegs; i++) {
            cp->rgs[i] = rgs[i];
        }
        cp->save_state = SAVE;
        //Jval val = new_jval_v(cp);
        //dll_prepend(readyq, val);
        cp = NULL;
        //fprintf(stdout, "save process %d %d....", rgs[PCReg], rgs[NextPCReg]);
    }
}

void syscall_return(PCB *p, int ret) {
   int buf[NumTotalRegs];
   p->rgs[PCReg] = p->rgs[NextPCReg];
   //fprintf(stdout, "syscall return %d %d....", p->rgs[PCReg], p->rgs[NextPCReg]);
   p->rgs[2] = ret; 
   Jval val = new_jval_v(p);
   dll_prepend(readyq, val);
   mt_exit();
}

void do_write(void *pv) {
   int ret = 0;
   PCB *p = (PCB *)pv;
   if (p->rgs[5] != 1 && p->rgs[5] != 2) {
       //fprintf(stdout, "arg1 %s\n", p->rgs[5]);
       syscall_return(p, 0 - EBADF);
   }
   //fprintf(stdout, "mem %d, loc %d\n", MemorySize, p->rgs[6]);
   if (p->rgs[6] < 0 || p->rgs[6] + p->rgs[7] > (p->limit)) {
       //fprintf(stdout, "arg1 %s\n", p->rgs[6]);
       syscall_return(p, 0 - EFAULT);
   }
   if (p->rgs[7] < 0) {
       syscall_return(p, 0 -EINVAL);
   }
   DEBUG('e', "1111111111\n");
   p->buf = main_memory + p->base + p->rgs[6];    
   p->buf_size = p->rgs[7];   
   mt_sem_down(writers);
   while (p->buf_size != p->chars_written) {
       console_write(p->buf[p->chars_written++]);
       mt_sem_down(writeok);
   } 
   mt_sem_up(writers);
   DEBUG('e', "2222222222\n");
   p->buf = main_memory + p->base + p->rgs[6];    
   p->buf = NULL;
   p->buf_size = 0;
   ret = p->chars_written;
   p->chars_written = 0;
   syscall_return(p, ret);
}

void do_read(void *pv) {
   PCB *p = (PCB *)pv;
   if (p->rgs[5] != 0) {
       syscall_return(p, 0 - EBADF);
   }
   if (p->rgs[6] < 0 || p->rgs[6] > p->limit) {
       syscall_return(p, 0 - EFAULT);
   }
   if (p->rgs[7] < 0) {
       syscall_return(p, 0 - EINVAL);
   }
   int ctoRead = 0, ret = 0;
   /** Put the char in to the read buffer */
   if (p->rbuf == NULL) {
       p->rbuf = main_memory +  p->base + p->rgs[6];    
       p->rbuf_chars = 0;
       ctoRead = p->rgs[7];
   }
   DEBUG('e', "ctoread %d\n", ctoRead);

   mt_sem_down(readers);
   while (p->rbuf_chars < ctoRead) { 
       mt_sem_down(nelem);
       /** Read the char from console buffer */
       int c = crb[crb_begin];
       DEBUG('e', "reading char %c\n", c);
       crb_begin++;
       crb_begin %= CRB_SIZE;
       crb_no_chars--;   
               
       //fprintf(stdout,"consumed char %d\n", c);
       p->rbuf[p->rbuf_chars++] = c;
       ret = p->rbuf_chars;
       if (c == EOF || c == 255 || c == -1) {
           ret = -1;
           //fprintf(stdout,"consumer break %d\n", ret);
           break;
       } else if (c == 10){
           break;
       } 
   }   
   //fprintf(stdout,"consumer returns %d\n", ret);
   p->rbuf = NULL;    
   p->rbuf_chars = 0;
   
   mt_sem_up(readers);
   syscall_return(p, ret);
}

void read_console(void *p) {
   //fprintf(stdout,"producing......\n");
   int i = 0;
   while (1) {
       mt_sem_down(consoleWait);
       //fprintf(stdout,"producer started......\n");
       int c = console_read();
       if (crb_no_chars < CRB_SIZE) {
           //fprintf(stdout,"produce char %d\n", c);
           crb[crb_end] = c;
           crb_end++; 
           crb[crb_end] = '\0';
           crb_end %= CRB_SIZE;
           crb_no_chars++;   
           //fprintf(stdout, "produce sequence: crb_begin: %d, crb_no_chars %d: ", crb_begin, crb_no_chars);
           int count = 0;
           i = crb_begin;
           while (count < crb_no_chars) {
               //fprintf(stdout, "%d", crb[i++]);
               i %= CRB_SIZE; 
               count++;
           }
           //fprintf(stdout, "\n");
           mt_sem_up(nelem);
       }      
   }
}

void do_ioctl(void *pv) {
   PCB *p = (PCB *)pv;
   if (p->rgs[5] != 1) {
       syscall_return(p, 0 - EINVAL);
   }
   if (p->rgs[6] != SOS_TCGETP) {
       syscall_return(p, 0 - EFAULT);
   }
   if (p->rgs[7] < 0) {
       syscall_return(p, 0 -EINVAL);
   }
   ioctl_console_fill(main_memory + p->base + p->rgs[7]);
   syscall_return(p, 0);
}

void do_fstat(void *pv) {
   PCB *p = (PCB *)pv;
   if (p->rgs[5] < 0) {
       syscall_return(p, 0 - EINVAL);
   }
   if (p->rgs[6] != SOS_TCGETP) {
       syscall_return(p, 0 - EINVAL);
   }
   if (p->rgs[7] < 0) {
       syscall_return(p, 0 -EINVAL);
   }
   int size = 0; 
   if (p->rgs[5] == 0) {
       size = 1;
   } else if (p->rgs[5] == 1 || p->rgs[5] == 2) {
       size = 256;
   }
   stat_buf_fill(main_memory + p->base + p->rgs[7]);
   syscall_return(p, 0);
}

void do_getpagesize(void *pv) {
   PCB *p = (PCB *)pv;
   syscall_return(p, PageSize);
}

void do_sbrk(void *pv) {
   PCB *p = (PCB *)pv;
   if (p->rgs[5] < 0) {
       syscall_return(p, 0 - EINVAL);
   }
   
   int incr = p->rgs[5];
   if ((p->sbrk + incr) > (p->limit)) {
       syscall_return(p, 0 - EINVAL); 
   }
   int temp = p->sbrk;
   p->sbrk = p->sbrk + p->rgs[5];       
   syscall_return(p, temp);
}

void syscall_return_1(PCB *p, int ret) {
   int buf[NumTotalRegs];
   //p->rgs[PCReg] = p->rgs[NextPCReg];
   //fprintf(stdout, "syscall return %d %d....", p->rgs[PCReg], p->rgs[NextPCReg]);
   p->rgs[2] = ret; 
   Jval val = new_jval_v(p);
   dll_prepend(readyq, val);
   mt_exit();
}

void do_execve(void *pv) {
   PCB *p = (PCB *)pv;
   char **argv = (char **)(main_memory + p->base + p->rgs[6]); 

   char *name = (char *) malloc(sizeof(char) * (strlen(main_memory + p->base + p->rgs[5]) + 1));
   memcpy(name, main_memory + p->base + p->rgs[5], strlen(main_memory + p->base + p->rgs[5]) + 1);

   DEBUG('v', "name %s\n", name);
   int i = 0, argc = 0; 
   if (argv != NULL) {
       while (argv[i] != NULL) {
          DEBUG('v', "argv %s\n", main_memory + p->base + (int)argv[i]);
          i++;        
       }   
       argc = i;
   }
   char **argvcopy = (char **) malloc(sizeof(char *) * (argc + 1));
   i = 0;
   if (argv != NULL) {
       while (argv[i] != NULL) {
          argvcopy[i] = (char *) malloc(sizeof(char) * (strlen(main_memory + p->base + (int)argv[i]) + 1));
          memcpy(argvcopy[i], main_memory + p->base + (int)argv[i], (strlen(main_memory + p->base + (int)argv[i]) + 1)); 
          DEBUG('v', "argv %s\n", argvcopy[i]); 
          i++;        
       }   
       argvcopy[i] = NULL;
   }
   int ret = 0;
   if (!(ret = perform_execve(p, name, argvcopy))) {
       syscall_return_1(p, 0);
   } else { 
       syscall_return(p, ret);
   }
}

int get_new_pid() {
    // Check for the lowest available pid
    JRB node;
    int i;
    printf("");
    for(i=1;i<32767;i++)
	if((node = jrb_find_int(jrbTree,i)) == NULL)
		break;
    curpid = i;
    DEBUG('e', "Available PID = %d\n", curpid); 
    (void) jrb_insert_int(jrbTree, i, new_jval_i(curpid));
    return curpid;
}

void destroy_pid(int pid) {
    JRB node;
    node = jrb_find_int(jrbTree,pid);
    if(node != NULL) {
        jrb_delete_node(node);
    }	
}

void do_getpid(void *pv) {
    PCB *p = (PCB *)pv;
    syscall_return(p, p->pid);
}

void init_partitions() {
    int i = 0;
    int partSize = 0;
    for (i = 0; i < 8; i++) {
       partitions[i].base = partSize;
       partSize += PageSize * 256;
       partitions[i].limit = PageSize * 256;
       //DEBUG('e', "8 %d\n", MemorySize/8);
       partitions[i].used = 0;
    }
}

PCB *copy_pcb(PCB *p) {
   PCB *newp = new_pcb();
   int i = 0;
   for (i = 0; i < NumTotalRegs; i++) {
       newp->rgs[i] = p->rgs[i];
   }
   newp->buf = NULL;
   newp->buf_size = 0;
   newp->chars_written = 0;
   
   newp->rbuf = NULL;
   newp->rbuf_chars = 0;
   newp->rbuf_size = 0;
   
   newp->sbrk = p->sbrk;
   newp->base = p->base;
   newp->limit = p->limit;
 
   newp->pid = p->pid;   
   return newp;
}

int get_partition() {
   int i = 0;
   for (i = 0; i < 8; i++) {
       DEBUG('f', "partition %d %d\n", i, partitions[i].used);
       if (partitions[i].used == 0) {
           partitions[i].used = 1;
           DEBUG('e', "partition %d base %d limit %d\n", i, partitions[i].base, partitions[i].limit);
           return i;
       }
   }
   return -1;
}

void finish_work(void *pv) {
   PCB *p = (PCB *)pv;
   syscall_return(p, 0);
}

void do_fork(void *pv) {
   PCB *p = (PCB *)pv;
	JRB node;		// Step 21
	int i;			// Step 21

   int partition = get_partition();
   if (partition == -1) {
       syscall_return(p, 0 - EAGAIN);
   }

   PCB *newp = copy_pcb(p);
   DEBUG('e', "********* partition %d, base %d limit %d", partition, partitions[partition].base, partitions[partition].limit);
   newp->limit = partitions[partition].limit;
   newp->base = partitions[partition].base;
   newp->mem_partition = partition;
   newp->parent = p;

   newp->pid = get_new_pid();
   DEBUG('e', "pid %d\n", newp->pid);
   memcpy(main_memory + newp->base, main_memory + p->base, p->limit);

	// Step 21 - Assign the non-zombie processes to the children RB Tree of the parent
	// keyed on the child's pid, and have the pcb as a value
	(void) jrb_insert_int(p->children, newp->pid, new_jval_v(newp));	

   mt_create(finish_work, newp);
   syscall_return(p, newp->pid);
}
  
void do_exit(void *pv) {
   PCB *p = (PCB *)pv;
   JRB node,ptr;		// Step 21 ,Step 22
   if (p == NULL) {
       DEBUG('e', "*******p NULL\n");
   }
   DEBUG('e', "cleaning partition %d\n", p->mem_partition);
   if (p == NULL) {
       DEBUG('e', "*******p NULL\n");
   }

   p->exit = p->rgs[5];
   partitions[p->mem_partition].used = 0;
   PCB *parent = p->parent;
   if (parent == NULL) {
       DEBUG('e', "*******parent NULL\n");
   }
   Jval val = new_jval_v(p);
   dll_prepend(parent->waiters, val);
   mt_sem_up(parent->waiter_sem);

   // Step 21 - remove the child from the parent's rb-tree when the child calls exit
   node = jrb_find_int(p->parent->children,p->pid);
   if (node != NULL)
   {
       DEBUG('e', "Removing child %d from patent's rb-tree\n",p->pid);
       jrb_delete_node(node);
   }

   // Step 22 - Transfer all the children of the dying process to init
   PCB *pcb;
   jrb_traverse(ptr, p->children)
   {
 	val = jrb_val(ptr);
        pcb = (PCB *)jval_v(val); 
	pcb->parent = init;
	(void) jrb_insert_int(init->children, pcb->pid, new_jval_v(pcb));	
        DEBUG('e', "Assigning child %d to init\n",pcb->pid);
   }

   
   Dllist dl;
   while (!dll_empty(p->waiters)) {
       dl = dll_prev(p->waiters);
       val = dll_val(dl);
       dll_delete_node(dl);
       pcb = (PCB *)jval_v(val);
       if (pcb) {
           destroy_pid(pcb->pid);
       }
   }

   // Step 23 - If the parent is init, the child should free itself.
   if(p->parent->pid == 0)
   {
        DEBUG('e', "The parent is init. Freeing up child %d\n",p->pid);
        free(p);
   }

   DEBUG('e', "exit done\n");
   mt_exit();      
}

void do_getdtablesize(void *pv) {
   PCB *p = (PCB *)pv;
   syscall_return(p, 64);
}

void do_getppid(void *pv) {
   PCB *p = (PCB *)pv;
   if (p == NULL) {
       DEBUG('e', "*******p NULL");
   }

   PCB *parent = p->parent;
   if (parent == NULL) {
       DEBUG('e', "*******parent NULL");
   }
   syscall_return(p, p->parent->pid);
}

void do_close(void *pv) {
   PCB *p = (PCB *)pv;
   syscall_return(p, 0 - EBADF);
}

void do_wait(void *pv) {
    PCB *p = (PCB *)pv;
    mt_sem_down(p->waiter_sem); 
    DEBUG('e', "child exit \n");
    Jval val;
    Dllist dl;
    if (dll_empty(p->waiters)) {
        syscall_return(p, 0); 
    } else {
        dl = dll_prev(p->waiters);
        val = dll_val(dl);
        dll_delete_node(dl);
         
        PCB *child = (PCB *)jval_v(val); 
        if (child != NULL) {
            int ret = child->exit;
            int pid = child->pid;
            //free(child);
            //TODO free PID
	    destroy_pid(pid);
            memcpy(main_memory + p->base + p->rgs[5], &ret, 4);
            DEBUG('e', "syscall return %d\n", ret);
            syscall_return(p, pid);  
        } else {
            DEBUG('e', "child empty\n");
        }              
    }   
}

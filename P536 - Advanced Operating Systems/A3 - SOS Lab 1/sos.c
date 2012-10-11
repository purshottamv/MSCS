/*
 * sos.c -- starting point for student's os.
 * 
 */

#include "simulator.h"

PCB currentRunningPCB = NULL;

void initialize_user_process(void *arg) 
{
	PCB thisPCB;
	char **argv1;
	int i = 0,j = 0,k = 0,l=0;
	int topofstack,diff;
    
	int *argptr;  // pointer to arguments

	int User_Base = 0;
	int User_Limit = MemorySize;
  
	argv1 = (char **)arg;
  
	thisPCB = (PCB) malloc(sizeof(struct ProcessControlBlock));
  
	if (load_user_program(argv1[0]) < 0)
	{
		fprintf(stderr,"Can't load program.\n");
		exit(1);
	}

	for (i=0; i < NumTotalRegs; i++) 
		thisPCB->registers[i] = 0;

	/* need to back off from top of memory */
	/* 12 for argc, argv, envp */
	/* 12 for stack frame */
	thisPCB->registers[PCReg] = 0; 
	thisPCB->registers[NextPCReg] = 4;
  
	while(argv1[j] != NULL) // count the number of command line arguments
		j++;     

	topofstack = User_Base + User_Limit - 20;

	argptr = (int *)malloc(sizeof(int)*j);
  
	for (k=j-1; k >= 0; k--) 
	{
		topofstack = topofstack - (strlen(argv1[k])+1);
		argptr[k] = topofstack - User_Base;
		strcpy(main_memory+topofstack, argv1[k]);
	}
  
	while(topofstack % 4) 
		topofstack--;

	topofstack =  topofstack - 4;
	l = WordToMachine(0);
	memcpy(main_memory + topofstack, &l, 4);

	// load pointers to strings
	for (k=j-1; k >= 0; k--) 
	{
		topofstack = topofstack - 4;
		l = WordToMachine(argptr[k]); 
		memcpy (main_memory + topofstack, &l, 4);
	}
	// pointer
	diff = topofstack - User_Base;
	// envp
	topofstack = topofstack - 4;
	l = WordToMachine(0);
	memcpy(main_memory + topofstack, &l, 4); 

	// argv
	topofstack = topofstack - 4;
	l = WordToMachine(diff);
	memcpy (main_memory + topofstack, &l, 4);

	// argc
	topofstack = topofstack - 4;
	l = j;
	memcpy(main_memory + topofstack, &l, 4);

	thisPCB->registers[StackReg] = topofstack - 12 - User_Base;		// stack pointer

	dll_append(readyq, new_jval_v(thisPCB));		// put user program at end of ready queue

	mt_exit();
}

void ConsoleRead()
{
	int i;
	while(1)
	{
		mt_sem_down(consoleWait);
		if(ConsoleReadBufferTail == -1)
		{
			ConsoleReadBufferTail = 0;
			ConsoleReadBufferHead = 0;
			ConsoleReadBuffer[ConsoleReadBufferTail] = console_read();
			mt_sem_up(nelem);
		}	
		else if((ConsoleReadBufferTail+1)%256 != ConsoleReadBufferHead)
		{
			ConsoleReadBufferTail = (ConsoleReadBufferTail + 1) % 256;
			ConsoleReadBuffer[ConsoleReadBufferTail] = console_read();
			mt_sem_up(nelem);
		}
	}
}

void scheduler()
{
	int i;

//	printf("\nscheduler() : Scheduler Called\n");
	Dllist firstDllist;
	PCB thisPCB;
	if(!dll_empty(readyq))
	{
		firstDllist = dll_first(readyq);	// Get the first PCB in the readyq to be scheduled next
		thisPCB = jval_v(dll_val(firstDllist));
		dll_delete_node(firstDllist);	// delete the node from the ready queue
		currentRunningPCB = thisPCB;
		DEBUG('e',"\n Removed first process from the ready queue \n");
		run_user_code(thisPCB->registers);
	}
	else
	{
		currentRunningPCB = NULL;
		DEBUG('e',"\n Ready Queue empty \n");
		noop();
	}
}

SOS()
{
	int i;
	static char *Argv[5] = { "argtest", "Rex,", "my", "man!", NULL };

	readyq = new_dllist();
	mt_init();
	writers = mt_sem_create(1);
	bzero(main_memory, MemorySize);
	ConsoleReadBufferHead = -1;

	readers = mt_sem_create(1);
	nelem = mt_sem_create(0);
	writeok = mt_sem_create(0);
	consoleWait = mt_sem_create(0);
	ConsoleReadBufferTail = -1;

	for( i= 0 ; i< CONSOLE_READ_BUFFER_SIZE ; i++)
		ConsoleReadBuffer[i] = -1;

	mt_create(ConsoleRead, NULL);
	mt_create(initialize_user_process, Argv);
	mt_joinall();
	scheduler();
}
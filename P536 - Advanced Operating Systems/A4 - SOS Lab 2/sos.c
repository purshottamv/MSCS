/*
 * sos.c -- starting point for student's os.
 * 
 */

#include "simulator.h"
#include "jval.h"
#include "dllist.h"
#include "process.h"
#include "mt.h"
#include <errno.h>

//static char *Argv[6] = {"argtest", "Rex", "my", "man!", "HELLO", NULL};
//static char *Argv[5] = {"errors", "Rex", "my", "man!", NULL};
//static char *Argv[2] = {"fork", NULL};
//static char *Argv[2] = {"exec", NULL};
//static char *Argv[2] = {"getppid", NULL};
//static char *Argv[2] = {"cat", NULL};
static char *Argv[2] = {"shell", NULL};
//static char *Argv[2] = {"hw2", NULL};

SOS()
{
        readyq = new_dllist();
        mt_init();
        writeok     = mt_sem_create(0);
        writers     = mt_sem_create(1);
        readers     = mt_sem_create(1);
        nelem       = mt_sem_create(0);
        consoleWait = mt_sem_create(0);
        
        crb_no_chars = 0;
        crb_end = 0; 
        crb_begin = 0;

        curpid = -1;
//      pids = make_rb();
        init_partitions(); 
        DEBUG('e', "pagesize: %d\n", PageSize);

	jrbTree = make_jrb();		// Step 20

	init = new_pcb();		// Step 22
	init->pid = get_new_pid();	// Step 22

        start_timer(10);	
	bzero(main_memory, MemorySize);
        mt_create(read_console, NULL);
        mt_create(initialize_user_process, (void *)Argv);
        schedule();
}

/*
 * exception.c -- stub to handle user mode exceptions, including system calls
 * 
 * Everything else core dumps.
 * 
 * Copyright (c) 1992 The Regents of the University of California. All rights
 * reserved.  See copyright.h for copyright notice and limitation of
 * liability and disclaimer of warranty provisions.
 */

#include "simulator.h"
#include "process.h"

void
exceptionHandler(ExceptionType which)
{
	int             type, r5, r6, r7, newPC;
	int             buf[NumTotalRegs];

	examine_registers(buf);
	type = buf[4];
	r5 = buf[5];
	r6 = buf[6];
	r7 = buf[7];
	newPC = buf[NextPCReg];
        
        PCB *p = cp;
        /* Save the registers */
        save_current_process_with_rgs(buf);

	/*
	 * for system calls type is in r4, arg1 is in r5, arg2 is in r6, and
	 * arg3 is in r7 put result in r2 and don't forget to increment the
	 * pc before returning!
	 */

	switch (which) {
	case SyscallException:
		/* the numbers for system calls is in <sys/syscall.h> */
		switch (type) {
		case 0:
			/* 0 is our halt system call number */
			DEBUG('e', "Halt initiated by user program\n");
			SYSHalt();
		case SYS_exit:
			/* this is the _exit() system call */
			DEBUG('e', "_exit() system call\n");
                        if (p != NULL) {
                            mt_create(do_exit, p);
                        }
                        break;
                case SYS_write: 
                        /* this should't be NULL */
                        DEBUG('e', "write sys call\n");
                        if (p != NULL) {
                            mt_create(do_write, p);
                        }
                        break;
                case SYS_read:
                        DEBUG('e', "read sys call\n");
                        if (p != NULL) {
                            mt_create(do_read, p);
                        }
                        break;
                case SYS_ioctl:
                        DEBUG('e', "ioctl sys call\n");
                        if (p != NULL) {
                            mt_create(do_ioctl, p);
                        }
                        break;
                case SYS_fstat:
                        DEBUG('e', "fstat sys call\n");
                        if (p != NULL) {
                            mt_create(do_fstat, p);
                        }
                        break;
                case SYS_getpagesize:
                        DEBUG('e', "getpagesize sys call\n");
                        if (p != NULL) {
                            mt_create(do_getpagesize, p);
                        }
                        break;
                case SYS_sbrk:
                        DEBUG('e', "sbrk sys call\n");
                        if (p != NULL) {
                            mt_create(do_sbrk, p);
                        }
                        break;
                case SYS_execve:
                        DEBUG('e', "execve sys call\n");
                        if (p != NULL) {
                            mt_create(do_execve, p);
                        }
                        break;
                case SYS_getpid:
                        DEBUG('e', "getpid sys call\n");
                        if (p != NULL) {
                            mt_create(do_getpid, p);
                        }
                        break;
                case SYS_fork:
                        DEBUG('e', "fork sys call\n");
                        if (p != NULL) {
                            mt_create(do_fork, p);
                        }
                        break;
                case SYS_getdtablesize:
                        DEBUG('e', "getdtablesize sys call\n");
                        if (p != NULL) {
                            mt_create(do_getdtablesize, p);
                        }
                        break;
                case SYS_getppid:
                        DEBUG('e', "getppid sys call\n");
                        if (p != NULL) {
                            mt_create(do_getppid, p);
                        }
                        break;
                case SYS_close:
                        DEBUG('e', "close sys call\n");
                        if (p != NULL) {
                            mt_create(do_close, p);
                        }
                        break;
                case SYS_wait:
                        DEBUG('e', "wait sys call\n");
                        if (p != NULL) {
                            mt_create(do_wait, p);
                        }
                        break;
		default:
			DEBUG('e', "Unknown system call\n");
			SYSHalt();
			break;
		}
		break;
	case PageFaultException:
		DEBUG('e', "Exception PageFaultException\n");
		break;
	case BusErrorException:
		DEBUG('e', "Exception BusErrorException\n");
		break;
	case AddressErrorException:
		DEBUG('e', "Exception AddressErrorException\n");
		break;
	case OverflowException:
		DEBUG('e', "Exception OverflowException\n");
		break;
	case IllegalInstrException:
		DEBUG('e', "Exception IllegalInstrException\n");
		break;
	default:
		printf("Unexpected user mode exception %d %d\n", which, type);
		exit(1);
	}
	schedule();
}

void
interruptHandler(IntType which)
{
        //fprintf(stdout, "interrupt");
        save_current_process();
	switch (which) {
	case ConsoleReadInt:
		DEBUG('i', "ConsoleReadInt interrupt\n");
                //fprintf(stdout, "ConsoleReadInt");
                //save_current_process();
                mt_sem_up(consoleWait);
		break;
	case ConsoleWriteInt:
		DEBUG('i', "ConsoleWriteInt interrupt\n");
                mt_sem_up(writeok);                 
		break;
        case TimerInt:
                break;                
	default:
		DEBUG('i', "Unknown interrupt\n");
		break;
	}
        schedule();
}


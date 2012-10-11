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
void syscall_return(PCB currentpcb, int x)
{
	DEBUG('e',"syscall_return Called.\n");
	currentpcb->registers[PCReg] = currentpcb->registers[NextPCReg]; // Set PCReg in the saved registers to NextPCReg.  
				// If you don't do this, the process will keep calling the system call. 
	currentpcb->registers[2] = x;  // Put the return value into the stored register 2
	dll_append(readyq,new_jval_v((void *)currentpcb));	// push the PCB onto the ready queue
	mt_exit();
}

void do_write(void *arg)
{
	int iRetVal = 1;
	char *arg2;
	PCB thisPCB = (PCB)arg;

  	//thisPCB -> chrPtr = thisPCB -> registers[6];
	thisPCB->size = thisPCB->registers[7];
	thisPCB->charswritten = 0;
	if(thisPCB->registers[5] != 1 && thisPCB->registers[5] != 2)
		iRetVal = -EBADF;

	if(thisPCB->registers[5] == 0)
		iRetVal = -EBADF;

	if(thisPCB->registers[6] < 0)
		iRetVal = -EFAULT;

	if(thisPCB->registers[6] > (MemorySize-24))
		iRetVal = -EFAULT;

	if(thisPCB->registers[7] < 0)
		iRetVal = -EINVAL;

	DEBUG('e', "Size %d, \n", thisPCB->size);
	arg2 = main_memory + thisPCB->registers[6];
	thisPCB->cptr = main_memory + thisPCB->registers[6];

	DEBUG('e', "before calling console write\n");
	mt_sem_down(writers);
	if(thisPCB->size > 0)
	{
		while(thisPCB->charswritten < thisPCB->size)
		{
			console_write(thisPCB->cptr[thisPCB ->charswritten]);
			mt_sem_down(writeok);
			thisPCB ->charswritten++;
		}
	}
	iRetVal = thisPCB->charswritten;
	DEBUG('e', "No of Characters written = %d\n", iRetVal);
	mt_sem_up(writers);
	syscall_return(thisPCB, iRetVal);
}

void do_read(void *arg)
{
	int iRetVal = 0;
	PCB thisPCB = (PCB)arg;
	mt_sem_down(readers);
	DEBUG('e', "Inside do_read\n");
	if(thisPCB->registers[5] != 1 && thisPCB->registers[5] != 2)
		iRetVal = -EBADF;

	if(thisPCB->registers[5] == 0)
		iRetVal = -EBADF;

	if(thisPCB->registers[6] < 0)
		iRetVal = -EFAULT;

	if(thisPCB->registers[6] > (MemorySize-24))
		iRetVal = -EFAULT;

	if(thisPCB->registers[7] < 0)
		iRetVal = -EINVAL;

	thisPCB->readbuffer = main_memory + thisPCB->registers[6];
	thisPCB->readsize = thisPCB->registers[7];
	thisPCB->charsread = 0;
	if(thisPCB->readsize > 0)
	{
		while(thisPCB->charsread < thisPCB->readsize)
		{
			mt_sem_down(nelem);		
		    if(ConsoleReadBuffer[ConsoleReadBufferHead] == -1)
				break;

			thisPCB->readbuffer[thisPCB->charsread] = ConsoleReadBuffer[ConsoleReadBufferHead];
			ConsoleReadBuffer[ConsoleReadBufferHead] = -1;
			if(ConsoleReadBufferHead == ConsoleReadBufferTail)
			{
				ConsoleReadBufferHead = -1;
				ConsoleReadBufferTail = -1;
			}
			else
			{
				ConsoleReadBufferHead = (ConsoleReadBufferHead + 1) % 256;
			}
			thisPCB->charsread++;   
		}
	}
	mt_sem_up(readers);
	syscall_return(thisPCB, thisPCB->charsread);
}

void
exceptionHandler(ExceptionType which)
{
	int             type, r5, r6, r7, newPC;
	int             buf[NumTotalRegs];

	int i;

	examine_registers(buf);

	type = buf[4];
	r5 = buf[5];	// argument 1
	r6 = buf[6];	// argument 2
	r7 = buf[7];	// argument 3
	newPC = buf[NextPCReg];

	if(currentRunningPCB != NULL)
	{
		for (i=0; i < NumTotalRegs; i++)
			currentRunningPCB->registers[i] = buf[i];
	}

	/*
	 * for system calls type is in r4, arg1 is in r5, arg2 is in r6, and
	 * arg3 is in r7 put result in r2 and don't forget to increment the
	 * pc before returning!
	 */
//	printf("exceptionHandler() : Exception Handler Called. Type %d\n",type);
	DEBUG('e', "Exception Handler Called. Type %d\n",type);
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
			printf("Program exited with value %d.\n", r5);
			SYSHalt();
		case SYS_write:
			DEBUG('e', "write system call\n");
			mt_create(do_write,(void *)currentRunningPCB);
			break;
		case SYS_read:
			DEBUG('e', "read system call\n");
			mt_create(do_read,(void *)currentRunningPCB);
			break;
		default:
			DEBUG('e', "Unknown system call\n");
			printf("System call no. %d\n",type);
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
	//noop();
	mt_joinall();
	scheduler();
}

void
interruptHandler(IntType which)
{
	int buf[NumTotalRegs],i;
	PCB programpcb;

	examine_registers(buf);

	if(currentRunningPCB != NULL)
	{
		for (i=0; i < NumTotalRegs; i++)
			currentRunningPCB->registers[i] = buf[i];
		dll_append(readyq,new_jval_v(currentRunningPCB));
	}

	DEBUG('e', "Interrupt handler called\n");	
	switch (which) {
	case ConsoleReadInt:
		DEBUG('e', "ConsoleReadInt interrupt\n");
		mt_sem_up(consoleWait);
		//noop();
		break;
	case ConsoleWriteInt:
		DEBUG('e', "ConsoleWriteInt interrupt\n");
		//noop();
		mt_sem_up(writeok);
		break;
	default:
		DEBUG('e', "Unknown interrupt\n");
		//noop();
		break;
	}
	mt_joinall();
	scheduler();
}

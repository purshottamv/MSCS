/*
 * simulator.h external declarations and stuff for the simulator.
 * 
 */
#include "dllist.h"
#include "mt.h"
#include "jval.h"
#include "errno.h"
#ifndef SIMULATOR_H

#define SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))

/* Boolean values. */

typedef enum {
FALSE = 0, TRUE = 1} bool;

/* machine exceptions */

typedef enum {
	SyscallException,		/* A program executed a system call. */
	PageFaultException,		/* Valid bit in PTE not set. */
	BusErrorException,		/* Translation gave invalid address. */
	AddressErrorException,		/* Unaligned or OOB addr ref. */
	OverflowException,		/* Integer overflow in add or sub. */
	IllegalInstrException,		/* Unimplemented or reserved instr. */

	NumExceptionTypes
}               ExceptionType;

/* exception handler called by the simulator with above type */

void            exceptionHandler(ExceptionType which);

/* interrupt types */

typedef enum {
	TimerInt, DiskInt, ConsoleWriteInt, ConsoleReadInt,
	NetworkSendInt, NetworkRecvInt
}               IntType;

/* interrupt handler called by the simulator when interrupt occurs */

void            interruptHandler(IntType which);

/* external variables which students can access memory */

#define PageSize                512
#define NumPhysicalPages        2048
#define MemorySize              (NumPhysicalPages * PageSize)

extern
char           *main_memory;

/* returns 0 if successful, -1 otherwise */
int             load_user_program(char *filename);

int             run_user_code(int registers[]);

/* functions to read/write to the console */

#define console_read		ConsoleGetChar
#define console_write(ch)	ConsolePutChar(ch)

/* functions and handy constants to read/write registers */

int             examine_registers(int buf[40]);

#define StackReg	29
#define RetAddrReg	31
#define NumGPRegs	32
#define HiReg		32
#define LoReg		33
#define PCReg		34
#define NextPCReg	35
#define PrevPCReg	36
#define LoadReg		37		/* The register target of a delayed
					 * load. */
#define LoadValueReg 	38		/* The value to be loaded by a
					 * delayed load. */
#define BadVAddrReg	39
#define NumTotalRegs 	40

/* function call to do nothing, NOTE: this function does NOT return */

int             noop();

/* system call numbers */

#define SYS_halt        0
#define SYS_exit        1
#define SYS_fork        2
#define SYS_read        3
#define SYS_write       4
#define SYS_close       6
#define SYS_wait        7
#define SYS_getpid      20
#define SYS_getppid     39
#define SYS_dup         41
#define SYS_pipe        42
#define SYS_ioctl       54
#define SYS_execve      59
#define SYS_fstat       62
#define SYS_getpagesize 64
#define SYS_sbrk        69
#define SYS_getdtablesize       89
#define SYS_dup2        90

typedef struct ProcessControlBlock
{
	char *cptr;
	int size;
	int charswritten;
	int charsread;
	int registers[NumTotalRegs];
	char *readbuffer;
	char *writebuffer;
	int readsize;
}*PCB;

extern PCB currentRunningPCB;
mt_sem writeok;
Dllist readyq;
mt_sem writers;
mt_sem readers;
mt_sem consoleWait;
mt_sem nelem;

#define CONSOLE_READ_BUFFER_SIZE 256
int ConsoleReadBuffer[CONSOLE_READ_BUFFER_SIZE];
int ConsoleReadBufferHead;
int ConsoleReadBufferTail;

#endif

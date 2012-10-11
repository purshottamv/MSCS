/*
 * simulator.h external declarations and stuff for the simulator.
 * 
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdlib.h>
#include <stdio.h>
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

/* returns sbrk(0) if successful, -1 otherwise */
int             load_user_program(char *filename);

void            run_user_code(int registers[]);

/* functions to read/write to the console */

#define console_read		ConsoleGetChar
#define console_write		ConsolePutChar
#define console_read2		ConsoleGetChar2
#define console_write2		ConsolePutChar2

extern
int	which_console;

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


/* base and bound registers */

extern
int             User_Base;
extern
int             User_Limit;

/* function call to do nothing, NOTE: this function does NOT return */

void            noop();

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


struct SOStermios {
        long            c_iflag;        /* Input Modes          */
        long            c_oflag;        /* Output Modes         */
        long            c_cflag;        /* Control Modes        */
        long            c_lflag;        /* Local Modes          */
        char            c_cc[19];       /* Control Characters   */
        char            c_line;         /* line disc. -local ext */
};

#define SOS_IOCPARM_MASK   0x7f         /* Parameters are < 128 bytes   */
#define SOS_IOC_VOID       (int)0x20000000      /* No parameters        */
#define SOS_IOC_OUT        (int)0x40000000      /* Copy out parameters  */
#define SOS_IOC_IN         (int)0x80000000      /* Copy in parameters   */
#define SOS_IOC_INOUT      (int)(SOS_IOC_IN|SOS_IOC_OUT)
#define SOS_IO(x,y)        (int)(SOS_IOC_VOID|(x<<8)|y)
#define SOS_IOR(x,y,t)     (int)(SOS_IOC_OUT|((sizeof(t)&SOS_IOCPARM_MASK)<<16)|(x<<8)|y)
#define SOS_IOW(x,y,t)     (int)(SOS_IOC_IN|((sizeof(t)&SOS_IOCPARM_MASK)<<16)|(x<<8)|y)
#define SOS_IOWR(x,y,t)    (int)(SOS_IOC_INOUT|((sizeof(t)&SOS_IOCPARM_MASK)<<16)|(x<<8)|y)

#define SOS_TCGETP          SOS_IOR('t',85,struct SOStermios)   /* Get parameters    */

struct SOSstat {
        short           st_dev;
        long            st_ino;
        short           st_mode;
        short           st_nlink;
        short           st_uid;
        short           st_gid;
        short           st_rdev;
        int             st_size;
        int             st_atime;
        int             st_spare1;
        int             st_mtime;
        int             st_spare2;
        int             st_ctime;
        int             st_spare3;
        long            st_blksize;
        long            st_blocks;
        unsigned long   st_gennum;
        long            st_spare4;
};

/*
void
                ioctl_console_fill(char *addr);
void
                stat_buf_fill(char *addr, int blk_size);
*/
#endif

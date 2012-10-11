#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


#if !defined(MT_H)
#define MT_H

#define	MT_NOERROR		0
#define	MT_ERROR		1
#define MT_MALLOCERROR	2
#define MT_CLONEERROR	3
#define	MT_INTHREAD		4
#define MT_SIGNALERROR	5
#define	MT_MAXTHREADS	6

#ifdef SOLARIS
#define JB_SP (1)
#define JB_FP (3)
#endif

#ifdef LINUX
#define JB_BP    3    
#define JB_SP    4    
#endif

#ifdef MACOSX
#define JB_SP 0
/* don't bother with FP */
#endif

/*
 * mt_init() initializes the library
 */ 
#ifdef _DEBUG
	#define _DEBUG_PRINT1( string ) fprintf( stdout, "MyThread debug: " string "\n")
	#define _DEBUG_PRINT2( string, arg ) fprintf( stdout, "MyThread debug: " string "\n", arg )
#else
	#define _DEBUG_PRINT1( string )
	#define _DEBUG_PRINT2( string, arg )
#endif

/* The maximum number of threads that can be active at once. */
#define MAX_THREADS 6000
#define INITIAL_MY_THREAD_IDENTIFIER 1000
/* The size of the stack for each thread. */
#define MT_STACK_SIZE (1024*1024)

typedef long unsigned int mythread_id;

typedef struct 
{
	time_t start_time;
	int sleepMode;
	int sleepDuration;
}sleep_info;

typedef struct
{
	mythread_id id;
	jmp_buf context;
	void *(*function)(void *);
	void *args;
	int runningStatus;
	void* stack;
	sleep_info si ;
} mythread_t;

/* The mythread unique identifier	*/
static mythread_id myThreadIdentifier = INITIAL_MY_THREAD_IDENTIFIER;

/* The mythread "queue" */
static mythread_t myThreadList[ MAX_THREADS ];

/* Main Thread sleep info - In case main thread executes mt_sleep */
static sleep_info sleepInfoMainThread;
/* The index of the currently executing thread */
static int currentThread = -1;
/* A boolean flag indicating if we are in the main process or if we are in a thread */
static int inThread = 0;
/* The total number of threads */
static int numThreads = 0;
/* The total number of active threads */
static int numWaitingThreads = 0;

/* The "main" execution context */
jmp_buf	mainThread;


void mt_init(void);	//Done

/*
 * returns my thread id
 */
mythread_id mt_self();	//Done


/*
 * mt_create() creates a new, runnable thread
 *
 * func: entry point for the new thread
 * arg: single argument to be passed
 */
int mt_create(void *(*func)(void *), void *arg);	//Done

/*
 * mt_yield() abdicates the CPU to that other runnable threads may execute
 */
void mt_yield();	//Done

/*
 * mt_join() blocks waiting for the completion of the thread specified
 *           in its argument list
 *
 * mt: the thread to wait for
 */
int mt_join(mythread_id);	//Done

/*
 * mt_joinall() blocks until there are not runnable threads left
 */
int mt_joinall();	//Done

/*
 * mt_exit() kills the calling thread immediately
 */
void mt_exit();		//Done

/*
 * mt_sleep(t) puts the thread to sleep for t seconds
 */
void mt_sleep(int secs);	//Done

/*
 * mt_kill() allows one thread to kill another
 */
int mt_kill(mythread_id);	//Done

/*
 **** Semaphore entry points start here
 */
//typedef void *mt_sem;
typedef struct 
{
	int sVal;
	int initVal;
	mythread_id threads[MAX_THREADS];
}mt_sem2;

typedef mt_sem2 *mt_sem;
/*
 * make_mt_sem() creates and initializes with its first argument which must
 *               be >= 0
 *               
 * initval: initial semaphore value
 */
mt_sem mt_sem_create(int initval);

/*
 * kill_mt_sem() deallocates the semaphore.  If there are threads blocked on
 *               the semaphore, kill_mt_sem() will cause the program to exit.
 *
 * ksem: semaphore to deallocate
 */
int mt_sem_destroy(mt_sem ksem);

/*
 * mt_getval(mt_sem s) retursn the current value
 */

int mt_sem_getval(mt_sem s);

/*
 * mt_sem_down() performs the P semaphore operation.  The current value is
 *            decremented and if it is < 0, the calling thread is queued on
 *            the semaphore and blocked
 *
 * ksem: relevant semaphore
 */
int mt_sem_down(mt_sem ksem);

/*
 * mt_sem_up() performs the V operation.  The value is incremented.  If it is 
 *            <= 0, a thread is dequeued and awakened.
 *
 * ksem: relevant semaphore
 */
int mt_sem_up(mt_sem ksem);

#endif
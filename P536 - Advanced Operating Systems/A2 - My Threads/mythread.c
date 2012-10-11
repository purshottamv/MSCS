/*
 ============================================================================
 Name        : mythread.c
 Author      : Purshottam Vishwakarma
 Version     : 1.0
 Copyright   : Copyrighted by Purshottam Vishwakarma
 Description : MyThread Library
 Date		 : September 23, 2011
 ============================================================================
 */
#include "mt.h"
#include <sys/time.h>

static void mtSignalHandler( int signum )
{
	assert( signum == SIGUSR1 );
	_DEBUG_PRINT2( "Signal handler for thread %d", numThreads );
	
	/* Save the current context, and return to terminate the signal handler scope */
	if ( setjmp( myThreadList[numThreads].context ) )
	{
		/* We are being called again from the main thread. Call the function */
		_DEBUG_PRINT2( "Starting thread %d", myThreadList[currentThread].id);
		myThreadList[currentThread].function(myThreadList[currentThread].args);
		_DEBUG_PRINT2( "Thread %d finished, returning to main", myThreadList[currentThread].id);
		myThreadList[currentThread].runningStatus = 0;
		longjmp( mainThread, 1 );
	}
	return;
}


void mt_init(void)
{
	int i;
	for ( i = 0; i < MAX_THREADS; ++ i )
	{
		myThreadList[i].stack = NULL;
		myThreadList[i].function = NULL;
		myThreadList[i].runningStatus = 0;

		myThreadList[ i ].si.sleepMode = 0;
		myThreadList[ i ].si.start_time = 0;
		myThreadList[ i ].si.sleepDuration = 0;
	}
		
	return;
}

mythread_id mt_self()
{
	return myThreadList[currentThread].id;
}

/*
 * mt_create() creates a new, runnable thread
 *
 * func: entry point for the new thread
 * arg: single argument to be passed
 */
int mt_create(void *(*func)(void *), void *arg)
{
	struct sigaction handler;
	struct sigaction oldHandler;
	
	stack_t stack;
	stack_t oldStack;
	_DEBUG_PRINT2( "Create the new stack : numThreads = %d",numThreads);
	
	if ( numThreads == MAX_THREADS ) return MT_MAXTHREADS;

	/* Create the new stack */
	stack.ss_flags = 0;
	stack.ss_size = MT_STACK_SIZE;
	stack.ss_sp = malloc( MT_STACK_SIZE );
	if ( stack.ss_sp == 0 )
	{
		_DEBUG_PRINT1( "Error: Could not allocate stack." );
		return MT_MALLOCERROR;
	}
	_DEBUG_PRINT2( "Stack address from malloc = %p", stack.ss_sp );

	/* Install the new stack for the signal handler */
	if ( sigaltstack( &stack, &oldStack ) )
	{
		_DEBUG_PRINT1( "Error: sigaltstack failed." );
		return MT_SIGNALERROR;
	}
	
	/* Install the signal handler */
	/* Sigaction *must* be used so we can specify SA_ONSTACK */
	handler.sa_handler = &mtSignalHandler;
	handler.sa_flags = SA_ONSTACK;
	sigemptyset( &handler.sa_mask );

	if ( sigaction( SIGUSR1, &handler, &oldHandler ) )
	{
		_DEBUG_PRINT1( "Error: sigaction failed." );
		return MT_SIGNALERROR;
	}
	
	/* Call the handler on the new stack */
	if ( raise( SIGUSR1 ) )
	{
		_DEBUG_PRINT1( "Error: raise failed." );
		return MT_SIGNALERROR;
	}
	
	/* Restore the original stack and handler */
	sigaltstack( &oldStack, NULL );
	sigaction( SIGUSR1, &oldHandler, NULL );
	
	/* We now have an additional thread, ready to roll */
	myThreadList[numThreads].runningStatus	= 1;
	myThreadList[numThreads].function		= func;
	myThreadList[numThreads].stack			= stack.ss_sp;
	myThreadList[numThreads].args			= arg;
	myThreadList[numThreads].id				= ++myThreadIdentifier;
	++ numThreads;
	++ numWaitingThreads;

	_DEBUG_PRINT2( "mt_create : Number of Threads = %d", numThreads );
	return myThreadIdentifier;
}

/*
  mt_yield() abdicates the CPU so that other runnable threads may execute
 */
void mt_yield()
{
	/* If we are in a thread, switch to the main context */
	if ( inThread )
	{
		/* Store the current state */
		if ( setjmp( myThreadList[ currentThread ].context ) )
		{
			/* Returning via longjmp (resume) */
			_DEBUG_PRINT2( "Thread %d resuming...", myThreadList[currentThread].id );
		}
		else
		{
			_DEBUG_PRINT2( "Thread %d yielding the processor to main...", myThreadList[currentThread].id );
			/* Saved the state: Let's switch back to the main state */
			longjmp( mainThread, 1 );
		}
	}
	/* If we are in main, dispatch the next Thread */
	else
	{
		if ( numWaitingThreads == 0 ) return;	// We don't have any threads waiting to execute
	
		/* Save the current state */
		if ( setjmp( mainThread ) )
		{
			/* The thread yielded the context to us */
			inThread = 0;
			if ( ! myThreadList[currentThread].runningStatus )
			{
				/* If we get here, the thread returned and is done! */
				_DEBUG_PRINT2( "thread %d returned, cleaning up.", myThreadList[currentThread].id );
				
				free( myThreadList[currentThread].stack );

				-- numWaitingThreads;
				/* Swap the last thread with the current, now empty, entry */
/*				-- numThreads;
				if ( currentThread != numThreads )
				{
					myThreadList[ currentThread ] = myThreadList[ numThreads ];
				}
				
				// Clean up the entry 
				myThreadList[numThreads].stack = 0;
				myThreadList[numThreads].function = 0;
				myThreadList[numThreads].runningStatus = 0;	*/
				// Clean up the entry 
				myThreadList[currentThread].stack = 0;
				myThreadList[currentThread].function = 0;
				myThreadList[currentThread].runningStatus = 0;

			}
			else
			{
				_DEBUG_PRINT2( "thread %d yielded execution.", myThreadList[currentThread].id );
			}
		}
		else
		{
			/* Saved the state so call the next thread */
			currentThread = (currentThread + 1) % numThreads;
			while(myThreadList[currentThread].runningStatus == 0 || 
				(myThreadList[ currentThread ].si.sleepMode = 1 && 
				(myThreadList[ currentThread ].si.start_time + myThreadList[ currentThread ].si.sleepDuration) > time(NULL)))
				currentThread = (currentThread + 1) % numThreads;

			_DEBUG_PRINT2( "Switching to thread %d", myThreadList[currentThread].id );
			inThread = 1;
			longjmp( myThreadList[ currentThread ].context, 1 );
		}
	}
	
	return;
}

int mt_joinall()
{
	int threadsRemaining = 0;
	
	_DEBUG_PRINT1( "mt_joinall() called.");

	/* If we are in a thread, wait for all the *other* threads to quit */
	if ( inThread ) threadsRemaining = 1;
	
	_DEBUG_PRINT2( "Waiting until there are only %d threads remaining...", threadsRemaining );
	
	while ( numWaitingThreads > threadsRemaining )
	{
		mt_yield();
	}
	
	return MT_NOERROR;
}

int mt_join(mythread_id identifier)
{
	int threadsRemaining = 0;
	
	_DEBUG_PRINT2( "mt_join() Called - Waiting for identifier = %d",identifier);

	/* If we are in a thread, wait for all the *other* threads to quit */
	if ( inThread ) threadsRemaining = 1;
	
	_DEBUG_PRINT2( "Waiting until there are only %d threads remaining...", threadsRemaining );
	
	while ( numWaitingThreads > threadsRemaining )
	{
		mt_yield();
		_DEBUG_PRINT2( "mt_join() - After yield - myThreadList[currentThread].id = %d",myThreadList[currentThread].id);
		_DEBUG_PRINT2( "mt_join() - After yield - identifier = %d",identifier);
		_DEBUG_PRINT2( "mt_join() - Checking running status of myThreadList[identifier-1001].runningStatus = %d",myThreadList[identifier-1001].runningStatus);
		//if(myThreadList[currentThread].id == identifier && myThreadList[currentThread].runningStatus == 0)
		//If the thread for which we are waiting is finished running then exit
		if(myThreadList[identifier-INITIAL_MY_THREAD_IDENTIFIER+1].runningStatus == 0) 
		{
			_DEBUG_PRINT2( "mt_join() - Completed execution of Thread id %d", identifier );
			break; 
		}
	}
	_DEBUG_PRINT2( "mt_join() - Exiting mt_join() - numWaitingThreads = %d",numWaitingThreads);
	_DEBUG_PRINT2( "mt_join() - Exiting mt_join()- threadsRemaining %d",threadsRemaining);
	return MT_NOERROR;
}

void mt_exit()
{
	_DEBUG_PRINT2( "Exiting Thread %d.", myThreadList[currentThread].id );
	
	myThreadList[currentThread].runningStatus = 0;	

	longjmp( mainThread, 1 );
}

int mt_kill(mythread_id threadid)
{

	int i=0,kill = 0;
	
	while(i<numThreads)
	{
		if(myThreadList[i].id == threadid && myThreadList[i].runningStatus == 1)
		{
			kill = 1;
			break;
		}
		i++;
	}
	if(kill == 1)
	{
		free( myThreadList[i].stack );

		-- numWaitingThreads;

		myThreadList[i].stack = 0;
		myThreadList[i].function = 0;
		myThreadList[i].runningStatus = 0;
		_DEBUG_PRINT2( "Killing Thread %d.", myThreadList[i].id );
		//longjmp( mainThread, 1 );
		return MT_NOERROR;
	}
	return MT_ERROR;
}

void mt_sleep(int secs)
{
	if ( inThread )  // When any of the thread calls sleep
	{	
		myThreadList[ currentThread ].si.sleepMode = 1;
		myThreadList[ currentThread ].si.start_time = time(NULL);
		myThreadList[ currentThread ].si.sleepDuration = secs;
		_DEBUG_PRINT2( "Going to Sleep Thread %d.", myThreadList[currentThread].id );
		mt_yield();
		_DEBUG_PRINT2( "Returning from Sleep Thread %d.", myThreadList[currentThread].id );
		myThreadList[ currentThread ].si.sleepMode = 0;
		myThreadList[ currentThread ].si.start_time = 0;
		myThreadList[ currentThread ].si.sleepDuration = 0;
	}
	else	// When main calls sleep
	{
		sleepInfoMainThread.sleepMode		= 1;          
		sleepInfoMainThread.start_time		= time(NULL);
		sleepInfoMainThread.sleepDuration	= secs;
		_DEBUG_PRINT1( "Going to Sleep Thread Main.");
		while((sleepInfoMainThread.start_time + sleepInfoMainThread.sleepDuration) > time(NULL))
			mt_yield();	// main thread yields
		_DEBUG_PRINT1( "Returning from Sleep Thread Main.");		
		sleepInfoMainThread.sleepMode		= 0;          
		sleepInfoMainThread.start_time		= 0;
		sleepInfoMainThread.sleepDuration	= 0;   
	}
	return;
}

mt_sem mt_sem_create(int initval)
{
	int i;
	if(initval>=0)
	{
		mt_sem sptr = (mt_sem)malloc(sizeof(mt_sem2));
		if(sptr != NULL)
		{
			sptr->initVal = initval;
			sptr->sVal = 0;
			for(i=0;i<MAX_THREADS;i++)
				sptr->threads[i] = -1;
			_DEBUG_PRINT2("Semaphore Created at %p",sptr );
			return sptr;
		}
		else
			return NULL;
	}
	else
		return NULL;
}

int mt_sem_destroy(mt_sem ksem)
{
	if(ksem != NULL)
	{
		_DEBUG_PRINT2("Semaphore destroyed at %p",ksem );
		free(ksem);
		return MT_NOERROR;
	}
	else
		return MT_ERROR;
}

int mt_sem_getval(mt_sem s)
{
	if(s != NULL)
		return s->sVal;
	else
		return 99999;
}

int mt_sem_down(mt_sem ksem)
{
	// Check if the same thread is trying to acquire the lock to enter the critical section
	// In that case don't allow it. Because it might create a deadlock
	int i;
	for(i=0;i<MAX_THREADS && ksem->threads[i] != -1;i++)
	{
		if(myThreadList[currentThread].id == ksem->threads[i])
		{	
			_DEBUG_PRINT2("Error Decrementing semaphore for thread %d",myThreadList[currentThread].id);
			return MT_ERROR;
		}
	}
	while(ksem->sVal < -(ksem->initVal))
	{
		mt_yield();
	}
	ksem->threads[-(ksem->sVal)] = 	myThreadList[currentThread].id;
	ksem->sVal--;
	_DEBUG_PRINT2("Decrementing semaphore to %d",ksem->sVal);
	return MT_NOERROR;
}

int mt_sem_up(mt_sem ksem)
{
	int i;
	// We increment the semaphore only if the thread had decremented the same semaphore before
	if(ksem->sVal < 0)
	{
		for(i=0;i<MAX_THREADS && ksem->threads[i] != -1;i++)
		{
			if(myThreadList[currentThread].id == ksem->threads[i])
			{	
				ksem->threads[i] = -1;
				ksem->sVal++;
				_DEBUG_PRINT2("Incrementing Semaphore to %d",ksem->sVal);
				return MT_NOERROR;
			}
		}
	}
	_DEBUG_PRINT2("Error Incrementing semaphore for thread %d",myThreadList[currentThread].id);
	return MT_ERROR;
}
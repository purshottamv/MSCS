#include <stdio.h>
#include <sys/time.h>
#include "mt.h"

int Debug=0;

#define FORKCOUNT (500)
#define PCOUNT (5000)

//#define FORKCOUNT (10)	// Number of threads created
//#define PCOUNT (10)		// Semaphore count down and count up


mythread_id t1;
mythread_id t2;

mt_sem s1;
mt_sem ws;

void foo()
{
	fprintf(stdout,"foo called, exiting\n");
	fflush(stdout);
	mt_exit();
}

void goo()
{
	fprintf(stdout,"goo calling foo\n");
	fflush(stdout);
	foo();
	fprintf(stdout,"goo back from foo -- problem\n");
	fflush(stdout);
}

void
Thread_1(void *arg)
{
	fprintf(stdout,"I am thread 1 and I'm printing: %s\n",arg);
	fflush(stdout);
	
	mt_sem_down(ws);
	mt_join(t2);

	fprintf(stdout,"I am thread 1 and I've joined t2\n");
	fflush(stdout);

	return;
}

void
Thread_2(void *arg)
{
	fprintf(stdout,"I am thread 2 and I'm printing: %s\n",arg);
	fflush(stdout);

	fprintf(stdout,"I am thread 2 and I'm calling goo\n");
	fflush(stdout);

	goo();

	return;
}

void
Thread_3(void *arg)
{
	fprintf(stdout,"I am thread 3 and I'm printing: %s\n",arg);
	fflush(stdout);


	return;
}

void Thread_4(void *arg)
{
	mt_sem s = (mt_sem)arg;

	fprintf(stdout,"I'm thread 4 and I'm synching with thread 5\n");
	fflush(stdout);

	mt_sem_down(s);

	fprintf(stdout,
		"I'm thread 4 and I'm back from synching with thread 5\n");
	fflush(stdout);

	return;
}

void Thread_5(void *arg)
{
	mt_sem s = (mt_sem)arg;

	fprintf(stdout,"I'm thread 5 and I'm synching with thread 4\n");
	fflush(stdout);

	mt_sem_up(s);

	fprintf(stdout,
		"I'm thread 5 and I'm back from synching with thread 4\n");
	fflush(stdout);

	mt_sem_up(s1);

	return;
}

void Sleep_T1(void *arg)
{
	int now = time(0);

	fprintf(stdout,"I'm a sleepy task and I'm sleeping for 3 seconds\n");
	fflush(stdout);
	mt_sleep(3);
	if((time(0) - now) < 3)
	{
		fprintf(stdout,
		"I'm a sleepy task and I woke up too quickly after %d secs\n",
	       		time(0) - now);
		fflush(stdout);
	}
	else
	{
		fprintf(stdout,"I'm a sleepy task 3 and I'm awake on time\n");
		fflush(stdout);
	}

	mt_exit();

	return;
}

void Sleep_T2(void *arg)
{
	int now = time(0);

	fprintf(stdout,"I'm a sleepy task and I'm sleeping for 5 seconds\n");
	fflush(stdout);
	mt_sleep(5);
	if((time(0) - now) < 5)
	{
		fprintf(stdout,
		"I'm a sleepy task and I woke up too quickly after %d secs\n",
	       		time(0) - now);
		fflush(stdout);
	}
	else
	{
		fprintf(stdout,"I'm a sleepy task 5 and I'm awake on time\n");
		fflush(stdout);
	}

	mt_exit();

	return;
}

void Y1(void *arg)
{
	int i;

	fprintf(stdout,"yielder 1 started\n");
	fflush(stdout);
	for(i=0; i < 10000000; i++);

	fprintf(stdout,"yielder 1 yielding\n");
	fflush(stdout);
	mt_yield();

	fprintf(stdout,"yielder 1 ending\n");
	fflush(stdout);

	for(i=0; i < 10000000; i++);

	mt_exit();
}

void Y2(void *arg)
{
	int i;

	fprintf(stdout,"yielder 2 started\n");
	fflush(stdout);
	for(i=0; i < 10000000; i++);

	fprintf(stdout,"yielder 2 yielding\n");
	fflush(stdout);
	mt_yield();

	fprintf(stdout,"yielder 2 ending\n");
	fflush(stdout);

	for(i=0; i < 10000000; i++);

	mt_exit();
}

void Forkee(void *arg)
{
	mt_exit();
	return;
}

void Forker(void *arg)
{
	void *t;
	t = mt_create(Forkee,NULL);
	mt_join(t);

	mt_exit();
	return;
}

void Per(void *a)
{
	int i;

	for(i=0; i < PCOUNT; i++)
	{
		mt_sem_down(a);
	}

	mt_exit();
}

void Ver(void *a)
{
	int i;

	for(i=0; i < PCOUNT; i++)
	{
		mt_sem_up(a);
	}

	mt_exit();
}



int
main()
{
	char *string1 = "Hi";
	char *string2 = "Mom";
	char *string3 = "How are you?";
	mythread_id t3;
	mt_sem s2;
	struct timeval start;
	struct timeval stop;
	int i;
	double elapsed;

	mt_init();
	ws = mt_sem_create(0);
	t1 = mt_create(Thread_1,string1);
	t2 = mt_create(Thread_2,string2);
	mt_sem_up(ws);

	mt_join(t1);
	fprintf(stdout,"main joined with thread 1\n");
	mt_sem_destroy(ws);
	fflush(stdout);

	t3 = mt_create(Thread_3,string3);

	fprintf(stdout,"t3 forked\n");
	fflush(stdout);

	mt_joinall();

	fprintf(stdout,"main back from joinall\n");
	fflush(stdout);

	s1 = mt_sem_create(1);
	s2 = mt_sem_create(0);

	t1 = mt_create(Thread_4,(void *)s2);

	fprintf(stdout,"main forked Thread_4 and calling P on s1\n");
	fflush(stdout);

	mt_sem_down(s1);

	fprintf(stdout,"main out of first P call\n");
	fflush(stdout);

	t2 = mt_create(Thread_5,(void *)s2);

	fprintf(stdout,"main forked thread 5 and calling P again\n");
	fflush(stdout);

	mt_sem_down(s1);

	fprintf(stdout,"main out of second P call\n");
	fflush(stdout);

	mt_joinall();

	mt_sem_destroy(s1);
	mt_sem_destroy(s2);

	fprintf(stdout,"main back from joinall, timing\n");
	fflush(stdout);

	fprintf(stdout,"%d fork/joins starting...",FORKCOUNT);
	fflush(stdout);
	gettimeofday(&start,NULL);
	for(i=0; i < FORKCOUNT; i++)
	{
		t1 = mt_create(Forker,NULL);
	}
	mt_joinall();
	gettimeofday(&stop,NULL);
	fprintf(stdout,"done\n");

	elapsed = ((double)((double)stop.tv_sec + 
		   (double)stop.tv_usec/1000000.0) -
	           (double)((double)start.tv_sec +
                   (double)start.tv_usec/1000000.0));
	fprintf(stdout,"%d fork/joins in %3.4fs == %3.4f fj/s\n",
			FORKCOUNT,
			elapsed,
			(double)FORKCOUNT/elapsed);
	fflush(stdout);

	fprintf(stdout,"main back from timing\n");
	fflush(stdout);

	fprintf(stdout,"%d P/V starting...",FORKCOUNT);
	fflush(stdout);

	s1 = mt_sem_create(0);

	gettimeofday(&start,NULL);
	t1 = mt_create(Per,(void *)s1);
	t2 = mt_create(Ver,(void *)s1);
	mt_joinall();
	gettimeofday(&stop,NULL);
	fprintf(stdout,"done\n");

	elapsed = ((double)((double)stop.tv_sec + 
		   (double)stop.tv_usec/1000000.0) -
	           (double)((double)start.tv_sec +
                   (double)start.tv_usec/1000000.0));
	fprintf(stdout,"%d P/Vs in %3.4fs == %3.4f pv/s\n",
			PCOUNT,
			elapsed,
			(double)PCOUNT/elapsed);

	fprintf(stdout,"main launching sleepers and yielders\n");
	fflush(stdout);

	mt_create(Sleep_T1,NULL);
	mt_create(Sleep_T2,NULL);

	mt_create(Y1,NULL);
	mt_create(Y2,NULL);

	t1 = mt_create(Y1,NULL);
	mt_yield();
	mt_kill(t1);

	fprintf(stdout,"thread Y1 killed\n");
	fflush(stdout);


	mt_joinall();

	fprintf(stdout,"main exiting\n");
	fflush(stdout);



	return(0);
}
/*
 * dphil_skeleton.c -- Dining philosophers driver program 
 */


#include <stdio.h>
#include "mt.h"
#include "dphil.h"

#define SUCCESS 1;
#define FAILED 0;

mythread_id threads[MAXTHREADS];
Phil_struct ps[MAXTHREADS];
mt_sem ws;	// Semaphore
int *blocktime;
char *s;

void *philosopher(void *v)
{
  Phil_struct *ps;
  long st;
  long t;

  ps = (Phil_struct *) v;
  while(1) {

    /* First the philosopher thinks for a random number of seconds */

    st = (random()%ps->ms) + 1;
	
    printf("%3d Philosopher %d thinking for %d second%s\n",
                time(0)-ps->t0, ps->id, st, (st == 1) ? "" : "s");
    fflush(stdout);
    mt_sleep(st);

    /* Now, the philosopher wakes up and wants to eat.  He calls pickup
       to pick up the chopsticks */

    printf("%3d Philosopher %d no longer thinking -- calling pickup()\n", 
            time(0)-ps->t0, ps->id);
    fflush(stdout);
    t = time(0);
    pickup(ps);
    ps->blocktime[ps->id] += (time(0) - t);

    /* When pickup returns, the philosopher can eat for a random number of
       seconds */

    st = (random()%ps->ms) + 1;
    printf("%3d Philosopher %d eating for %d second%s\n",
                time(0)-ps->t0, ps->id, st, (st == 1) ? "" : "s");

    fflush(stdout);
    mt_sleep(st);

    /* Finally, the philosopher is done eating, and calls putdown to
       put down the chopsticks */

    printf("%3d Philosopher %d no longer eating -- calling putdown()\n", 
            time(0)-ps->t0, ps->id);
    fflush(stdout);
    putdown(ps);
  }
}


void *initialize_v(int phil_count, int sleeptime)
{
	int i;
	blocktime = (int *) malloc(sizeof(int)*phil_count);
  

  for (i = 0; i < phil_count; i++) {
    ps[i].id = i;
    ps[i].t0 = time(0);;
    ps[i].v = NULL;
	ps[i].ms = sleeptime; 
	ps[i].blocktime = blocktime;
    ps[i].phil_count = phil_count;
    threads[i] = mt_create(philosopher, (void *) (ps+i));
	blocktime[i] = 0;
	ps[i].s = THINKING;
  }
	// Create the semaphore
	ws = mt_sem_create(0);

}

int test(int i)
{
	if( (ps[(i+4) % 5].s != EATING) &&
		(ps[i].s == HUNGRY) &&
		(ps[(i+1) % 5].s != EATING) )
	{
		return SUCCESS;
	}
	else
	{
		return FAILED;
	}
}

void pickup(Phil_struct *ph)
{

	ps[ph->id].s = HUNGRY;
	while(!test(ph->id))
	{
		mt_yield();
	}
	mt_sem_down(ws);
	ph->s = EATING;
	mt_sem_up(ws);
}

void putdown(Phil_struct *ph)
{
	mt_sem_down(ws);
	ph->s = THINKING;
	mt_sem_up(ws);
}

typedef void (*sighandler_t)(int);

void handle_signal(int signalno)
{
	printf("Exiting the Program - freeing all allocated memory\n");
	if(mt_sem_destroy(ws))
		printf("semaphore destroy failed\n");
	free(s);
	free (blocktime);
	exit(1);
	return;
}

main(argc, argv)
int argc; 
char **argv;
{
  int i;
  void *v;
  long t0;

  s = (char *)malloc(sizeof(char)*500 + 1);
  s[500] = '\0';
  int phil_count;
  char *curr;
  int total;

  if (argc != 3) {
    fprintf(stderr, "usage: dphil philosopher_count maxsleepsec\n");
    exit(1);
  }
  else if(atoi(argv[2]) < 1)
  {
    fprintf(stderr, "Enter maxsleepsec >= 1\n");
	exit(1);
  }

	printf("\n\n********    Starting Dining Philosopher's Problem - Enter Ctrl-C to exit.   ***********\n\n");
  srandom(time(0));

  signal(SIGINT,handle_signal);

  phil_count = atoi(argv[1]);
  
  if(phil_count > MAXTHREADS)
	  phil_count = MAXTHREADS;
   
	initialize_v(phil_count,atoi(argv[2]));
	t0 = time(0);
	ws = mt_sem_create(0);	// Creating the binary semaphore

  while(1) {
    curr = s;
    total=0;
    for(i=0; i < phil_count; i++)
	    total += ps[i].blocktime[i];
    sprintf(curr,"%3d Total blocktime: %5d : ", 
		    time(0)-t0,
		    total);
    curr = s + strlen(s);
    for(i=0; i < phil_count; i++)
    {
    	sprintf(curr, "%5d ", ps[i].blocktime[i]);
		curr = s + strlen(s);
    }
    printf("%s\n", s);
    fflush(stdout);
    mt_sleep(10);
  }

}
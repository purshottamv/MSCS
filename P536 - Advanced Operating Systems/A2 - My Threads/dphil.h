enum state
{
	THINKING=0,
	HUNGRY,
	EATING
};
typedef struct {
  int id;                /* The philosopher's id: 0 to 5 */
  long t0;               /* The time when the program started */
  long ms;               /* The maximum time that philosopher sleeps/eats */
  void *v;               /* The void * that you define */
  int *blocktime;      /* Total time that a philosopher is blocked */
  int phil_count;
  enum state s;
} Phil_struct;

extern void *initialize_v(int phil_count,int sleeptime);
extern void pickup(Phil_struct *);
extern void putdown(Phil_struct *);

#define MAXTHREADS (100)
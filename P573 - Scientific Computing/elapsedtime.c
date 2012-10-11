//--------------------------------------------------------------------------------
// 
// Example timer for use from within C/C++
// 
// Note that this is actually a call to some system clock and multiple versions
// are provided via commenting/uncommenting out blocks below. The function
// gettimeofday() is on all systems, but generally a high resolution clock is a
// better choice if one is available. Read warnings in the comments about 
// nonstandard or nonportable usages.
//
// The convention used is: a negative number is returned in case the 
// clock_gettime() call fails, instead of the error flag from that function.
//
// The makefile links in librt, but it is not needed for all of the timers
// here. Modify the makefile and pick one of the timers that don't need
// the real-time library if it is not found.
// 
//----------------
// Randall Bramley
// Department of Computer Science
// Indiana University, Bloomington
//----------------
// Started: Fri 28 Sep 2007, 06:38 PM
// Modified: Tue 01 Sep 2009, 04:40 PM for monotone option
// Modified: Mon 27 Aug 2012, 10:04 AM for p573
// Last Modified: Mon 27 Aug 2012, 10:05 AM
//--------------------------------------------------------------------------------

//-------------------------------------
// Version 1: using clock_gettime()
//-------------------------------------
// #include <time.h>
// 
// double elapsedtime(void)
// {
//     int flag;
//     clockid_t cid = CLOCK_REALTIME; // CLOCK_MONOTONE might be better
//     struct timespec tp;
//     double timing;
// 
//     flag = clock_gettime(cid, &tp);
//     if (flag == 0) 
//         timing = tp.tv_sec + 1.0e-9*tp.tv_nsec;
//     else 
//         timing = -17.0;  // If timer failed, return non-valid time
//     
//     return(timing);
// 
// };

//---------------------------------
// Version 2: using gettimeofday()
//---------------------------------
// #include <time.h>
// #include <stddef.h>
// #include <sys/time.h>
// #include <unistd.h>                                                                  
// 
// double elapsedtime()
// {
//     struct timeval t;
//     struct timezone whocares;
//     double total;
//     double sec, msec;     /* seconds */
//     double usec;          /* microseconds */
// 
//     // gettimeofday(&t, NULL);
//     gettimeofday(&t, &whocares);
// 
//     msec = (double) (t.tv_sec);
//     usec = 1.0e-6*(double) (t.tv_usec);
//     total = msec + usec;
//     if (total < 0) 
//         return(-17.0);
//     else
//         return(total);
// 
// }

//------------------------------------------------------------------------
// Version 3: using times(). This uses sysconf() to get ticks per second.
// Returns a double that gives the sum of user and system time.
//------------------------------------------------------------------------
// #include <sys/times.h>
// #include <unistd.h>                                                                  
// #include <stdio.h>
// 
// double elapsedtime(void)
// {
//     struct tms p;
//     clock_t now;
//     long sumpun;
// 
//     sumpun = sysconf(_SC_CLK_TCK);
//     clock_t ret = times(&p);
//     return( (double) (p.tms_utime + p.tms_stime)/(double) sumpun );
// }

//------------------------------------------------------------------------------
// Version 4: using the cpu timer clock()
// This will not count sleep time, so testing this needs to do actual CPU work.
//------------------------------------------------------------------------------
// #include <sys/types.h>
// #include <time.h>
// #include <unistd.h>
// #include <stdio.h>
// double elapsedtime(void)
// {
//     clock_t now ;
//     long cps;
//     //------------------------------------------------------------------
//     // Alternate (and recommended) way to get clock tics per second is:
//     //------------------------------------------------------------------
//     // long sumpun;
//     // sumpun = sysconf(_SC_CLK_TCK);
//     // return ((double) now)/(double) sumpun;
// 
//     cps = CLOCKS_PER_SEC;
//     now = clock();
//     return ((double) now)/(double) cps;
// 
// }

//------------------------------------------------------------------------------
// Version 5: using a high resolution non-standard clock
// Beware: this hardcodes in the CPU Ghertz rate, which is specific to a single
// machine. On systems with the proc filesystem, rate can be obtained from 
//    cat /proc/cpuinfo
//------------------------------------------------------------------------------
 #include <stdio.h> 
 unsigned long long int cycles_x86_64(void)
 {
   unsigned long long int val;
   do {
      unsigned int a, d;
      asm volatile("rdtsc" : "=a" (a), "=d" (d));
      (val) = ((long long)a) | (((long long)d)<<32);
   } while(0);
   return(val);
 }
 
 double elapsedtime(void)
 {
     unsigned long long int val;
     double retval;
     double cpurate = 3.2e9;
 
     val = cycles_x86_64();
     retval = (double) val/ cpurate;
     if (retval < 0.0)
         return(-17.0);
     else
         return(retval);
 }

//-----------------------------------------
// Version 6: another HRC, not tested yet
//-----------------------------------------
// #define CPS 2327505000;
// static double iCPS;
// static unsigned start=0;
// 
// double second(void) /* Include an '_' if you will be calling from Fortan */
// {
//   double foo;
//   if (!start)
//   {
//      iCPS = 1.0/(double)CPS;
//      start = 1;
//   }
//   foo = iCPS*cycles_x86_64();
//   return(foo);
// }

//***************************************************************************/
// Version 7: uses subfields in the process time structure; use
//      man -s3 times
// to see what those subfields are. This form allows including/excluding the
// time spent in child processes, etc.
//***************************************************************************/

// #include <sys/times.h>  // for struct tms defn
// #include <time.h>
// #include <unistd.h>                                                                  
// double elapsedtime()
// {
//     struct tms p;
//     long sumpun = sysconf(_SC_CLK_TCK);
//     clock_t now;
// 
//     now = times(&p);
//     return( (double) (p.tms_utime + p.tms_stime)/(double) sumpun);
// }



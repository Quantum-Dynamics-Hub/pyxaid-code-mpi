// OpenMP thread-safe C and C++ timing routines
// Dave Turner - Kansas State University
// This is free software distributed under the GPL license

// C    #undef USE_CPP               below 
//      #include "mytimer_c.h"       in all C files where timing is to be used
//      Timer_Start( "blockname" );  starts the timer for 'blockname'
//      Timer_Stop( );               stops the timer for the current block
//      Timer_Report( "time.out" );  dumps the time info to file 'time.out'
//      Timer_Log_Flops( nflops );   logs nflops floating ops to current block
//      Timer_Log_Bytes( nbytes );   logs nbytes bytes communicated within block

// Start the timer with Timer_Start("name"), and stop with Timer_Stop()
// If you log nflops Timer_Report() will calculate the GFlops rate
// If you log nbytes Timer_Report() will calculate the MB/sec comm rate
// Call Timer_Report( "time.out" ) once at the end of the run


// C++  #define USE_CPP 1            below 
//      #include "mytimer_cpp.h"     in all C++ files where timing is to be used
//      Timer timer( "blockname" );  starts the timer for 'blockname'
//      timer.Start( "section" );    starts the timer manually for a section
//      timer.Stop( );     stops the timer manually for a section
//      the ~Timer() destructor will stop the timer automatically for a block
//      timer.Report( "time.out" );  dumps the time info to file 'time.out'
//      Timer.Log_Flops( nflops );   logs nflops floating ops to current block
//      Timer.Log_Bytes( nbytes );   logs nbytes bytes communicated within block

// Timer timer( "name" )   The constructor will start the timer
// The destructor will stop the timer for that block automatically
// If you want to time a separate block of code (loop, IO section, communication
//    you can bracket the block with timer.Start("name") and timer.Stop()


// f90  Compile in the mytimer_f90.90 module and this mytimer.c file using OpenMP
//        for gcc compile mytimer.c with -fopenmp
//        for icc compile mytimer.c with -openmp
//      USE mytimer_f90              in all sections where the timer is used
//      Timer_Start( "blockname" );  starts the timer for 'blockname'
//      Timer_Stop( );               stops the timer for the current block
//      Timer_Report( "time.out" );  dumps the time info to file 'time.out'
//      Timer_Log_Flops( nflops );   logs nflops floating ops to current block
//      Timer_Log_Bytes( nbytes );   logs nbytes bytes communicated within block
//         nflops and nbytes should be 64-bit integers


// USAGE NOTE: For MPI only rank 0 will normally call Timer_Report()


// Define USE_CPP as 1 and `ln -s mytimer.c mytimer.cpp` to use C++
#undef USE_CPP
#define USE_CPP 1
#ifdef USE_CPP
#include "mytimer_cpp.h"  // Need the class definition
//#else
//#include "mytimer_c.h"
#endif

#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define NUM_TIMED_BLOCKS 1000

struct timed_block {
   char name[100];         // name from input parameters
   double t_enter;         // when did entry occur
   double total_time;      // total time spent in routine and children
   double child_time;      // total time spent in lower routines
   int64_t ncalls;         // number of entries into this block of code
   int64_t nflops;         // total number of floating-point operations
   int64_t nbytes;         // total number of Bytes communicated
   int dotiming;           // True if timing is more than clock overhead
};

static int max_block = 0;
static int stack_ptr = -1;
static int stack[NUM_TIMED_BLOCKS];  // Stack of block id numbers
static struct timed_block block[NUM_TIMED_BLOCKS];
static double clock_overhead = 0.0; 
static int do_not_time = 0;         // Turn timing on or off ( 0 || 1 )

// Use the best clock dependent on the system

// os x, compile with: gcc -o testo test.c
// linux, compile with: gcc -o testo test.c -lrt
 
double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time
 
#ifdef __MACH__ // Use clock_get_time for OSX
   clock_serv_t cclock;
   mach_timespec_t mts;
   host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
 
   clock_get_time(cclock, &mts);
 
   mach_port_deallocate(mach_task_self(), cclock);

   if( t_start == 0 ) t_start = mts.tv_sec;

   return (double) (mts.tv_sec - t_start) + mts.tv_nsec * 1.0e-9; 
#elif 1       // Use this nanosecond clock for now
   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9; 
#else         // This is a usec clock
   struct timeval ts;

   gettimeofday(&ts, (struct timezone *)NULL);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return ( (double)(ts.tv_sec - t_start) + ((double)ts.tv_usec)*1.0e-6);
#endif
}
 
// Return the clock overhead time in usecs
// DDT - Not accurate yet as increasing ntests decreases the resulting overhead

double get_clock_overhead()
{
   volatile double t;
   double overhead = 0.0;
   int i, ntests = 1000;

      // Set manually to 0.1 usec for now - measured externally

   return 0.000000100;

   for( i=0; i<ntests; i++) {
      t = myclock();
      overhead += ( myclock() - t );
      //printf(" %lf usecs\n", overhead*1.0e6 );
   }
   return overhead/ntests;
}


// Constructor or manual start of the timer

void TimerStart( char const * blockname )
{
#pragma omp master    // Make thread-safe
{
   int i, id = -1;
   struct timed_block *blk;

   if( do_not_time ) goto bail; // return that works from within master thread

//printf("Master inside Timer( %s )\n", blockname);
      // First time we call Timer() - Calculate clock overhead

   if( clock_overhead == 0.0 ) {
      clock_overhead = get_clock_overhead();
   }

      // Find the id for this blockname if it exists

   for( i=0; i<max_block; i++) {
      if( strcmp( blockname, block[i].name ) == 0) {
         id = i;
         blk = &block[id];
         break;
      }
   }

   if(id == -1) {         // Add a new timing block if needed
      id = max_block++;
      if( id >= NUM_TIMED_BLOCKS ) {
         printf("ERROR - Increase the number of timed blocks in myTimer.cpp\n");
         exit(1);
      }
      blk = &block[id];
      strcpy( blk->name, blockname );
      blk->total_time = 0.0;
      blk->child_time = 0.0;
      blk->ncalls = 0;
      blk->nflops = 0;
      blk->nbytes = 0;
      blk->dotiming = 1;  // Set dotiming true.  Retest after 100 timings.
   }

      // Add it to the stack and log timing information

   stack[++stack_ptr] = id;
   blk->ncalls++;

      // Start timer only if function takes longer than the clock overhead

   if( blk->dotiming ) {
      blk->t_enter = myclock();
   }

bail:; }
}

// Destructor or manual stop of the timer.
// Ignore destructor call if block is already stopped manually.

void TimerStop()
{
#pragma omp master    // Make thread-safe
{
   int id;
   volatile double t, dt = 0.0;
   struct timed_block *blk;

   if( do_not_time ) goto bail;  // return that works from within master thread

//printf("Master inside ~Timer( )\n");
   if( stack_ptr < 0 ) {
     fprintf(stderr,"WARNING - ~Timer() has no matching constructor\n");
     goto bail;
   }

   id = stack[stack_ptr--];
   blk = &block[id];

   if( blk->dotiming ) {
      t = myclock();
      dt  += ( t - blk->t_enter );
   }

      // Add this time to the current block and to the parent's child_time

   blk->total_time += dt;
   if(stack_ptr >= 0) {
      block[stack[stack_ptr]].child_time += dt;
   }

      // Check after 100 calls to see if we need to turn timing off

   //if( blk->ncalls == 100 ) {
      //if( blk->total_time < 100 * clock_overhead * 10 ) {
         //blk->dotiming = 0;  // Turn off timing as it is intrusive
         //blk->total_time = 0.0;
      //}
   //}
bail:; }
}

// Add Flops count into current timed block

void TimerLogFlops( int64_t nflops )
{
#pragma omp master    // Make thread-safe
{
   int id;

   if( do_not_time ) goto bail;  // return that works from within master thread

   if( stack_ptr >= 0 ) {
      id = stack[stack_ptr];
      block[id].nflops += nflops;
   }
bail:; }
}

// Add Byte count into current timed block

void TimerLogBytes( int64_t nbytes )
{
#pragma omp master    // Make thread-safe
{
   int id;

   if( do_not_time ) goto bail;  // return that works from within master thread

   if( stack_ptr >= 0 ) {
      id = stack[stack_ptr];
      block[id].nbytes += nbytes;
   }
bail:; }
}

// Timer_Report sorts the times and dumps to timefile

void TimerReport( char const * timefile)
{
#pragma omp master    // Make thread-safe
{
   double t, dt = 0.0, frac, total_time, seconds;
   int64_t total_nflops = 0;
   struct timed_block *blk;
   int id, jd, hours, minutes;
   FILE *fp;
   char hostname[100];

//printf("Master inside Timer_Report( ) stack_ptr = %d\n", stack_ptr);

   if( do_not_time ) goto bail;  // return that works from within master thread

// Close all existing timing blocks

   while( stack_ptr >= 0 ) {

      id = stack[stack_ptr--];
      blk = &block[id];

      if( blk->dotiming ) {
         t = myclock();
         dt  += ( t - blk->t_enter );
      } else {
         dt = 0.0;
      }

         // Add this time to the current block and to the parent's child_time

      blk->total_time += dt;
      if(stack_ptr >= 0) {
         block[stack[stack_ptr]].child_time += dt;
      }
   }

// Dump info to the time file, append if one already exists

   fp = fopen( timefile, "a" );

   total_time = block[0].total_time;

   if( total_time < 1.0e-9 ) {
      fprintf(fp,"Total time is zero for some reason\n");
      goto bail;  // return that works from within master thread
   }

      /* Sort by descending time spent in each block */

   for(id=0; id<max_block-1; id++) {

      dt = block[id].total_time - block[id].child_time;

      for(jd=id+1; jd<max_block; jd++) {

         if( block[jd].total_time - block[jd].child_time > dt ) {

            dt = block[jd].total_time - block[jd].child_time;
            block[max_block] = block[id];
            block[id] = block[jd];
            block[jd] = block[max_block];

   }   }   }

   fprintf(fp,"    time in seconds    +children       calls    name\n");

   for(id=0; id<max_block; id++) {

      dt = block[id].total_time - block[id].child_time;
      if( dt < 0.0 ) dt = 0.0;
      frac = 100 * dt / total_time;

      fprintf(fp, " %10.3f (%5.1f%%) %10.3f %12ld  %-20.20s",
         dt, frac, block[id].total_time, block[id].ncalls, block[id].name);

         // Print out GFlops or MB/sec rates if present

      if( block[id].nflops > 0 && dt > 0.0 ) {
         total_nflops += block[id].nflops;
         fprintf(fp,"  %7.3f GFlops\n",block[id].nflops * 1.0e-9 / dt);
      } else if( block[id].nbytes > 0  && dt > 0.0) {
         fprintf(fp,"  %9.2f MB/sec\n",block[id].nbytes * 1.0e-6 / dt);
      } else {
         fprintf(fp,"\n");
      }
   }

   if( total_nflops > 0 ) {
      fprintf(fp,
       "\n %9.2f billion operations on each processors --> %9.3f GFlops/proc\n",
          total_nflops * 1.0e-9, total_nflops *1.0e-9 / total_time);
   }

   fprintf(fp, "\n  Total time is %lf secs  ", total_time );
   (void) gethostname( hostname, 100);
   hours   = (int) (total_time / 3600.0);
   minutes = (int) ( (total_time - hours * 3600.0) / 60.0 );
   seconds =         (total_time - hours * 3600.0 - minutes * 60.0 );
   fprintf(fp, "  (%d hours  %d mins  %6.3lf secs) on %s\n", 
               hours, minutes, seconds, hostname);

   //fprintf(fp,   "  Timer overhead ~ %lf usecs\n", clock_overhead * 1.0e6 );
   fprintf(fp, "  Timer overhead ~ %lf usecs  no cutoff for now\n\n\n", 
               clock_overhead * 1.0e6 );

   fclose(fp);

bail:; }
}

#ifdef USE_CPP   // C++ headers for all functions

Timer::Timer( char const * blockname ) : __blockname( blockname )
{ TimerStart( blockname ); }

void Timer::Start( char const * subblockname ) // Manually start timer
{ TimerStart( subblockname ); }

// Object destructor will do nothing if timer already stopped for this block
Timer::~Timer()
{
   if( stack_ptr < 0 ) return;

   int id = stack[stack_ptr];
   if( strcmp( __blockname, block[id].name) == 0 ) {
      TimerStop( );
   }
}

void Timer::Stop()   // Manually stop a timed section
{ TimerStop( ); }

void Timer::Log_Flops( int64_t nflops )
{ TimerLogFlops( nflops ); }

void Timer::Log_Bytes( int64_t nbytes )
{ TimerLogBytes( nbytes ); }

void Timer::Report( char const * timefile)
{ TimerReport( timefile ); }

#else       // C stubbs

void Timer_Start( char const * blockname ) { TimerStart( blockname ); }

void Timer_Stop() { TimerStop( ); }

void Timer_Log_Flops( int64_t nflops ) { TimerLogFlops( nflops ); }

void Timer_Log_Bytes( int64_t nbytes ) { TimerLogBytes( nbytes ); }

void Timer_Report( char const * timefile) { TimerReport( timefile ); }

#endif


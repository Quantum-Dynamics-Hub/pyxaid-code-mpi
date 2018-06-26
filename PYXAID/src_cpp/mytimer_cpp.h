// This is free software distributed under the GPL license
#ifndef TIMER_H
#define TIMER_H 1

#include <stdint.h>  // for int64_t

// Time duration between the construction and destruction of this timer object

class Timer
{
   public:
      Timer( char const * );            // Start timer with constructor
      ~Timer();                         // Stop timer with destructor
      void Start( char const * );       // Start timer manually
      void Stop();                      // Stop timer manually
      void Log_Flops( int64_t );        // Set number of floating ops in block
      void Log_Bytes( int64_t );        // Set num bytes communicated in block
      void Report( char const *);  // Dumping timing info to a file
   private:
      char const * __blockname;
};

#endif

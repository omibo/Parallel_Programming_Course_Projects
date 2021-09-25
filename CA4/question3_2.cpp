#include <stdio.h>
#include <math.h>
#include <sys/time.h>
// #include <sys/resource.h>
#include <omp.h>

double timeGetTime()
{
	struct timeval time;
	struct timezone zone;
	gettimeofday(&time, &zone);
	return time.tv_sec + time.tv_usec*1e-6;
}

char ColorCodes[5][250] = {
	"\033[1m\033[33m",		// Bold YELLOW
	"\033[1m\033[31m",		// Bold RED
	"\033[1m\033[32m",		// Bold GREEN
	"\033[1m\033[34m",		// Bold BLUE
	"\x1B[0m"				// Reset
};


const long int VERYBIG = 100000;

int main( void )
{
  int i;
  long int j, k, sum;
  double sumx, sumy, total, z;
  double starttime, elapsedtime;
  double tstarttime, telapsedtime;
  // ---------------------------------------------------------------------
  // Output a start message
  printf("\033[1m\033[35mOmid Bodaghi -- 810196423\tMobina Shahbandeh -- 810196488\x1B[0m\n\n");
  printf( "OpenMP Parallel Timings for %ld iterations \n\n", VERYBIG );

  // repeat experiment several times
  for( i=0; i<6; i++ )
  {
    // get starting time
    starttime = timeGetTime();
    // reset check sum and total
    sum = 0;
    total = 0.0;

    // Work loop, do some work by looping VERYBIG times
    #pragma omp parallel \
    num_threads (4) \
    private( sumx, sumy, k, j, tstarttime, telapsedtime )
    {
      tstarttime = timeGetTime();
      #pragma omp for \
        reduction( +: sum, total ) \
        schedule( static ) \
        nowait

        for( int j=0; j<VERYBIG; j++ )
        {
          // increment check sum
          sum += 1;

          // Calculate first arithmetic series
          sumx = 0.0;
          for( k=0; k<j; k++ )
          sumx = sumx + (double)k;

          // Calculate second arithmetic series
          sumy = 0.0;
          for( k=j; k>0; k-- )
          sumy = sumy + (double)k;

          if( sumx > 0.0 )total = total + 1.0 / sqrt( sumx );
          if( sumy > 0.0 )total = total + 1.0 / sqrt( sumy );
        }
      telapsedtime = timeGetTime() - tstarttime;
      printf("%sTime Elapsed for T%d:%10d mSecs\n",
                  ColorCodes[omp_get_thread_num()],
                  omp_get_thread_num(), (int)(telapsedtime * 1000));
    }

    // get ending time and use it to determine elapsed time
    elapsedtime += timeGetTime() - starttime;
    printf ("%s\n", ColorCodes[4]);
  }
  // report elapsed time
  printf("Average Time Elapsed %10d mSecs Total=%lf Check Sum = %ld\n",
                   (int)(elapsedtime / 6 * 1000), total, sum );
  // return integer as required by function header
  return 0;
}
// **********************************************************************

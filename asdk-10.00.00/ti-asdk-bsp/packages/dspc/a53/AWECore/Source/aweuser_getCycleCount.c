#include <time.h>
#include <stdint.h>
#include "StandardDefs.h"

            
UINT32 aweuser_getCycleCount(void){
	/* User defined function to return the cycle count to the AWECore library to enable profiling
	*  Must return a 32-bit integer
	*  The function can be inlined to improve performance if the target supports it
	*/
	
	/* Code below implements a functioning cycle counter on Linux platforms using the system call clock_gettime()
	*	This can be used if the target supports clock_gettime, but non-system methods to return the CPU's cycle counter
	*	are preferred when available due to performance penalties of system calls. 
	*/
	//struct timespec tv;
	//clock_gettime(CLOCK_REALTIME, &tv);
	//int64_t usec = (tv.tv_sec * 10000000) + (tv.tv_nsec / 100);
	//return (usec & 0xffffffff);
	
	return 0;
}

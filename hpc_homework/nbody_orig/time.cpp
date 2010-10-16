#include "time.h"
#ifndef WIN32

#include <sys/time.h>

double get_time(){
        timeval tim;
        gettimeofday(&tim,0);
        return tim.tv_sec+(tim.tv_usec/1000000.0);
}

#else

double get_time(){
	return 0;
}

#endif


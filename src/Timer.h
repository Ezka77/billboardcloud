
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Timer_h_ 
#define _Timer_h_

#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

class Timer
{
public:

    typedef double Time;

    Timer()
    {
        reset();
    }
    
    ~Timer()
    {
    }
    
    inline void start(void)
    {
        mLast = getTime();
    }
    inline void stop(void)
    {
        mSum += getTime() - mLast;
    }
    inline void reset(void)
    {
        mSum = 0.;
    }
    
    inline Time value(void)
    {
        return mSum;
    }
    
    static Time getTime(void)
    {
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        return (Time) ((double)tv.tv_sec + 1.e-6 * (double)tv.tv_usec);
    }

protected:

    Time mSum, mLast;

};


#endif


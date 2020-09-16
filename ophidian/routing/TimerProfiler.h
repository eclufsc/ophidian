/* 
 * File:   Timer.h
 * Author: Nima (nkarimpo@ucalgary.ca)
 *
 * Created on March 15, 2016, 6:04 PM
 */

#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>  
#include <sys/resource.h> 
#include <unistd.h>

#define USE_GETTIMEOFDAY

namespace UCal
{
    

class TimerProfiler {
public:
    TimerProfiler():
        m_time1(0.0),
        m_time2(0.0)
    { }
    virtual ~TimerProfiler(){}
public:
    inline double userTime( void ) const 
    { 
        return (m_time2-m_time1);
    } 
       
    inline void start() 
    { 
#ifdef USE_GETTIMEOFDAY
        m_time1 = ( gettimeofday(&m_tv,  NULL) == 0 )
                    ? m_tv.tv_sec + 1.0e-6 * m_tv.tv_usec
                    : 0.0
                    ;
#else
        m_time1 = ( getrusage( RUSAGE_SELF, &m_ru ) == 0 ) 
                    ? m_ru.ru_utime.tv_sec + 1.0e-6 * m_ru.ru_utime.tv_usec
                    : 0.0
                    ; 
#endif
    }

    inline void stop () 
    { 
#ifdef USE_GETTIMEOFDAY
        m_time2 = ( gettimeofday(&m_tv,  NULL) == 0 )
                    ? m_tv.tv_sec + 1.0e-6 * m_tv.tv_usec
                   : 0.0
                   ;
#else
        m_time2 = ( getrusage( RUSAGE_SELF, &m_ru ) == 0 ) 
                    ? m_ru.ru_utime.tv_sec + 1.0e-6 * m_ru.ru_utime.tv_usec
                    : 0.0
                   ; 
#endif
    }
    
    
protected:

#ifdef USE_GETTIMEOFDAY
    struct timeval m_tv;
#else
    struct rusage m_ru;

#endif
        
    double m_time1;
    double m_time2;

};


}
#endif /* TIMER_H */


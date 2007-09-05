#ifndef STOPWATCH_H
#define STOPWATCH_H STOPWATCH_H


#include<sys/timeb.h>


class Stopwatch {
public:
    inline void start();
    inline unsigned long long readSeconds() const;
    inline unsigned long long readMilliseconds() const;

private:
    timeb start_;
};

void Stopwatch::start() {
    ftime( &start_ );
}

unsigned long long Stopwatch::readSeconds() const {
    timeb now;
    ftime( &now );
    return now.time - start_.time;
}

unsigned long long Stopwatch::readMilliseconds() const {
    timeb now;
    ftime( &now );
    return ( now.time - start_.time ) * 1000 + ( now.millitm - start_.millitm );
}


#endif

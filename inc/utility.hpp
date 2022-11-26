#ifndef UTILITY_H
#define UTILITY_H

#include "preh.hpp"

// standard data type
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned long ULONG;

// data structure data type
typedef std::vector<UINT> UINTVEC;
typedef std::vector<double> DBLEVEC;
typedef std::list<UINT> UINTLIST;
typedef std::list<double> DBLELIST;

// scope based timer
class Timer
{
public:
    // constructor
    Timer();
    // destructor
    ~Timer();
    // stop timer
    void Stop() const noexcept;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

#endif
#include "../inc/utility.hpp"

// Benchmark timer class definition here
// constructor
Timer::Timer()
{
    // gets the current time point
    m_start = std::chrono::high_resolution_clock::now();
}

// destructor
Timer::~Timer()
{
    // stop the timer and show results
    Stop();
}

void Timer::Stop() const noexcept
{
    // gets the current time point
    std::chrono::time_point<std::chrono::high_resolution_clock> end = \
        std::chrono::high_resolution_clock::now();

    auto start = std::chrono::time_point_cast<std::chrono::microseconds> \
        (m_start).time_since_epoch().count();
    auto stop = std::chrono::time_point_cast<std::chrono::microseconds> \
        (end).time_since_epoch().count();

    auto duration = stop - start;
    double ms = duration * 0.001;

    std::cout << duration << " us\n";
    std::cout << ms << " ms\n";
}
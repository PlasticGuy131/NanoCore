#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED 1

/// @brief Initializes clock
void clock_initialize();

/// @brief Increments the clock counter
void clock_increment();

/// @brief Sleeps for the specified amount of time
/// @param time Time to sleep in ms
void clock_sleep(unsigned time);

#endif
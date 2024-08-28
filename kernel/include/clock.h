#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED 1

/// @brief Increments the clock counter
void clock_increment() { clock++; }

/// @brief Returns the value of the clock counter
/// @return Current value of the clock
int clock_time() { return clock; }

#endif
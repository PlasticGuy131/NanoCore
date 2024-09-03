#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED 1

/// @brief Initialises the basic processor state
void general_initialize();

/// @brief Prints out initialization values
/// @param init Name of processing being initialized
/// @param success If initialization was a success
/// @param sleep Insert wait for dramatic effect?
void init_print(const char* init, bool success, bool sleep);

#endif
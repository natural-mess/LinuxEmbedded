/** @file common.h
 *  @brief Common definitions (e.g., data structs)
 *
 *  Contains node data format that are used
 *  throughout project
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef _COMMON_H
#define _COMMON_H

#include "../src/sbuffer.h"

#define MAX_SENSORS 50
#define TIMEOUT_SECONDS 15

volatile sig_atomic_t shutdown = 0;

#endif /* _COMMON_H */
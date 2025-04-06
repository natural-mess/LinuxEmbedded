/** @file threads.h
 *  @brief Threads management
 *
 *  Manage the creation and detachment of the three threads 
 *  (connection, data, storage managers).
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef THREADS_H
#define THREADS_H

#include "sbuffer.h"

typedef struct
{
    sbuffer_t* sb;
    int port;
} thread_args_t;

void init_threads(sbuffer_t* sb, int port);

#endif /* THREADS_H */
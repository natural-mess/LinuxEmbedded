#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "sbuffer.h"
#include "log.h"
#include "threads.h"

#define MAX_RETRIES 3

void* storage_manager(void* arg);

#endif /* STORAGE_MANAGER_H */

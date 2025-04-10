/** @file log.h
 *  @brief Declarations for logging functions
 *
 *  Defines functions for the log process and sending log events via FIFO
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#ifndef LOG_PROCESS_H
#define LOG_PROCESS_H

#define LOG_FIFO "/tmp/logFifo" // Default FIFO name
#define LOG_FIFO_PATH "logs/gateway.log"

// Runs the log process to read from FIFO and write to log file
void log_process_run(const char *fifo_path, const char *log_file);

// Sends a log message to the FIFO
void log_event(const char *msg);

#endif /* LOG_PROCESS_H */

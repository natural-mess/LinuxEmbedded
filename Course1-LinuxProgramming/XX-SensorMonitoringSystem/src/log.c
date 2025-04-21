/** @file log.c
 *  @brief Implementation of the log process
 *
 *  Handles logging events received via FIFO and writes them to gateway.log
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

// Runs the log process to read from FIFO and write to log file
void log_process_run(const char *fifo_path, const char *log_file)
{
    // Ensure the log file directory exists
    char dir_path[256];
    snprintf(dir_path, sizeof(dir_path), "%s", log_file);
    char *last_slash = strrchr(dir_path, '/');
    if (last_slash != NULL)
    {
        *last_slash = '\0'; // Trim to directory path
        if (access(dir_path, F_OK) == -1)
        {
            if (mkdir(dir_path, 0777) == -1 && errno != EEXIST)
            {
                perror("Failed to create log directory");
                exit(1);
            }
        }
    }

    // Create named pipe FIFO if it doesn't exist
    if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST)
    {
        perror("Failed to create named pipe fifo in log_process_run");
        exit(1);
    }

    // Open FIFO for reading
    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd == -1)
    {
        perror("Failed to open FIFO for reading");
        exit(1);
    }

    // Open log for appending
    FILE *log_fp = fopen(log_file, "a+");
    if (log_fp == NULL)
    {
        perror("Failed to open log file");
        close(fifo_fd);
        exit(1);
    }

    // Ensure the file has the correct permissions
    chmod(log_file, 0666); // Set permissions to match FIFO

    // static variable to track number of log entries
    static unsigned int seq_num = 1;

    // Buffer to read log messages from FIFO
    char buffer[4096]; // Increased buffer size to handle larger reads
    char *line;
    char *saveptr = NULL;
    ssize_t bytes_read;

    // Loop to read from FIFO and write to log file
    while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0';

        // Process each line in the buffer
        line = strtok_r(buffer, "\n", &saveptr);
        while (line != NULL)
        {
            // Get current time
            time_t now;
            time(&now);
            char time_str[26];
            ctime_r(&now, time_str);
            time_str[strlen(time_str) - 1] = '\0';

            // Write log entry
            fprintf(log_fp, "%u %s %s\n", seq_num++, time_str, line);
            fflush(log_fp);

            // Get the next line
            line = strtok_r(NULL, "\n", &saveptr);
        }
    }

    // If read fails or EOF, exit
    if (bytes_read == -1)
    {
        perror("Error reading from FIFO");
        log_event("FIFO read error, shutting down log process\n");
    }

    // Clean up
    if (fclose(log_fp) != 0)
    {
        perror("Failed to close log file");
    }
    if (close(fifo_fd) != 0)
    {
        perror("Failed to close FIFO");
    }

    exit(0);
}

// Sends a log message to the FIFO
void log_event(const char *msg)
{
    static int fifo_fd = -1;
    static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

    if (pthread_mutex_lock(&log_mutex) != 0)
    {
        perror("Log mutex lock failed");
        return;
    }

    // Initialize FIFO if not already open
    if (fifo_fd == -1)
    {
        // Open FIFO in read-write mode so writer doesn't fail if no reader is attached
        fifo_fd = open(LOG_FIFO, O_RDWR | O_NONBLOCK);
        if (fifo_fd == -1)
        {
            perror("Failed to open FIFO for writing");
            pthread_mutex_unlock(&log_mutex);
            return;
        }
    }

    // Create message with newline
    char msg_with_newline[256];
    snprintf(msg_with_newline, sizeof(msg_with_newline), "%s\n", msg);

    // Write message to FIFO
    if (fifo_fd != -1)
    {
        ssize_t bytes_written = write(fifo_fd, msg_with_newline, strlen(msg_with_newline));
        if (bytes_written == -1)
        {
            perror("Failed to write to FIFO");
        }
    }

    if (pthread_mutex_unlock(&log_mutex) != 0)
    {
        perror("Log mutex unlock failed");
        return;
    }
}
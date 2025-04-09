/** @file storage_manager.c
 *  @brief Implementation of the storage manager
 *
 *  Stores sensor data into a SQLite database.
 *
 *  @author Phuc
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <unistd.h>
#include "storage_manager.h"
#include "common.h"
#include "log.h"
#include "threads.h"

static const char *DB_PATH = "db/sensors.db";
#define MAX_RETRIES 3

void *storage_manager(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    sbuffer_t *sb = args->sb;
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    // Open or create the database
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK)
    {
        log_event("Failed to open database");
        goto cleanup;
    }
    log_event("Connection to SQL server established");

    // Create table if it doesn't exist
    static const char *CREATE_TABLE = "CREATE TABLE IF NOT EXISTS measurements (id INTEGER, temp REAL, time INTEGER);";
    char *err_msg = NULL;
    if (sqlite3_exec(db, CREATE_TABLE, NULL, NULL, &err_msg) != SQLITE_OK)
    {
        printf("Failed to create table: %s\n", err_msg);
        log_event("Failed to create table");
        sqlite3_free(err_msg);
        goto cleanup;
    }
    log_event("New table measurements created or already exists");

    // Main loop
    while (!shutdown_flag)
    {
        sensor_data_t data;

        // Check for shutdown before blocking on sbuffer_pop
        if (shutdown_flag)
            break;

        // Pop data from sbuffer with retry logic
        int pop_retries = 0;
        while (pop_retries < MAX_RETRIES && sbuffer_pop(sb, &data) != 0)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Failed to pop data from sbuffer, retry %d/%d", pop_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1); // Brief pause before retry
            pop_retries++;
        }

        if (pop_retries == MAX_RETRIES)
        {
            log_event("Max retries reached for popping data, skipping...");
            continue;
        }

        // Prepare insert statement
        static const char *INSERT_STMT = "INSERT INTO measurements VALUES (?, ?, ?);";
        int prepare_retries = 0;
        while (prepare_retries < MAX_RETRIES && sqlite3_prepare_v2(db, INSERT_STMT, -1, &stmt, NULL) != SQLITE_OK)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Failed to prepare insert statement, retry %d/%d", prepare_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1); // Brief pause before retry
            prepare_retries++;
        }

        if (prepare_retries == MAX_RETRIES)
        {
            log_event("Max retries reached for preparing SQL statement, skipping this data point.");
            continue;
        }

        // Bind values
        if (sqlite3_bind_int(stmt, 1, data.sensor_id) != SQLITE_OK ||
            sqlite3_bind_double(stmt, 2, data.temperature) != SQLITE_OK ||
            sqlite3_bind_int64(stmt, 3, (sqlite3_int64)data.timestamp) != SQLITE_OK)
        {
            log_event("Failed to bind values to SQL statement");
            goto cleanup_stmt;
        }

        // Execute statement with retries
        int step_retries = 0;
        while (step_retries < MAX_RETRIES && sqlite3_step(stmt) != SQLITE_DONE)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Failed to insert row, retry %d/%d", step_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1); // Brief pause before retry
            step_retries++;
        }

        if (step_retries == MAX_RETRIES)
        {
            log_event("Max retries reached for inserting row, skipping this data point.");
            goto cleanup_stmt;
        }

        log_event("Inserted a row to SQL table");

    cleanup_stmt:
        if (stmt)
        {
            sqlite3_finalize(stmt);
            stmt = NULL;
        }

        // Check for shutdown again after processing
        if (shutdown_flag)
            break;
    }

cleanup:
    if (stmt)
        sqlite3_finalize(stmt);
    if (db && sqlite3_close(db) != SQLITE_OK)
    {
        log_event("Failed to close database");
    }
    log_event("Storage manager shutting down");

    return NULL;
}

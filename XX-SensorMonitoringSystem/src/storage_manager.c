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
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include "storage_manager.h"
#include "../include/common.h"
#include "log.h"
#include "threads.h"

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#define PATH_SEPARATOR "\\"
#else
#include <errno.h>
#define mkdir(path, mode) mkdir(path, mode)
#define PATH_SEPARATOR "/"
#endif

static const char *DB_DIR = "db";
static const char *DB_NAME = "sensors.db";

static void sqlite_error_log_callback(void *pArg, int iErrCode, const char *zMsg)
{
    char msg[512]; // Increased size for safety
    snprintf(msg, sizeof(msg), "SQLite error %d: %s", iErrCode, zMsg);
    log_event(msg);
}

void *storage_manager(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    sbuffer_t *sb = args->sb;
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    // Enable SQLite error logging
    sqlite3_config(SQLITE_CONFIG_LOG, sqlite_error_log_callback, NULL);
    sqlite3_initialize();

    // Construct database path
    char db_path[256];
    snprintf(db_path, sizeof(db_path), "%s%s%s", DB_DIR, PATH_SEPARATOR, DB_NAME);

    // Log the database path for debugging
    char msg[512]; // Increased from 256 to 512 to avoid truncation warnings
    snprintf(msg, sizeof(msg), "Attempting to open database at %s", db_path);
    log_event(msg);

    // Ensure the directory exists
    if (access(DB_DIR, F_OK) == -1)
    {
        if (mkdir(DB_DIR, 0777) == -1 && errno != EEXIST)
        {
            perror("Failed to create database directory");
            snprintf(msg, sizeof(msg), "Failed to create database directory: %s", strerror(errno));
            log_event(msg);
            return NULL;
        }
    }

    // Open or create the database
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK)
    {
        snprintf(msg, sizeof(msg), "Failed to open database at %s: %s", db_path, sqlite3_errmsg(db));
        log_event(msg);
        printf("SQLite error: %s\n", sqlite3_errmsg(db));
        if (db)
            sqlite3_close(db);
        return NULL;
    }

    snprintf(msg, sizeof(msg), "Connected to database %s", db_path);
    log_event(msg);
    time_t now = time(NULL);
    char time_str[26];
    ctime_r(&now, time_str);
    time_str[strlen(time_str) - 1] = '\0';
    printf("%s: Connected to database %s\n", time_str, db_path);

    // Create table if it doesn't exist
    static const char *CREATE_TABLE =
        "CREATE TABLE IF NOT EXISTS measurements ("
        "id INTEGER NOT NULL, "
        "temp REAL NOT NULL, "
        "time INTEGER NOT NULL"
        ");";
    char *err_msg = NULL;
    if (sqlite3_exec(db, CREATE_TABLE, NULL, NULL, &err_msg) != SQLITE_OK)
    {
        snprintf(msg, sizeof(msg), "Failed to create table: %s", err_msg);
        log_event(msg);
        printf("Failed to create table: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return NULL;
    }
    log_event("New table measurements created or already exists");
    now = time(NULL);
    ctime_r(&now, time_str);
    time_str[strlen(time_str) - 1] = '\0';
    printf("%s: Table measurements ready\n", time_str);

    // Main loop
    while (!shutdown_flag)
    {
        sensor_data_t data;

        if (shutdown_flag)
            break;

        int pop_retries = 0;
        while (pop_retries < MAX_RETRIES && sbuffer_pop(sb, &data) != 0)
        {
            if (shutdown_flag)
                goto cleanup;
            snprintf(msg, sizeof(msg), "Failed to pop data from sbuffer, retry %d/%d", pop_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1);
            pop_retries++;
        }

        if (pop_retries == MAX_RETRIES)
        {
            log_event("Max retries reached for popping data, skipping...");
            continue;
        }

        static const char *INSERT_STMT = "INSERT INTO measurements VALUES (?, ?, ?);";
        int prepare_retries = 0;
        while (prepare_retries < MAX_RETRIES && sqlite3_prepare_v2(db, INSERT_STMT, -1, &stmt, NULL) != SQLITE_OK)
        {
            snprintf(msg, sizeof(msg), "Failed to prepare insert statement, retry %d/%d", prepare_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1);
            prepare_retries++;
        }

        if (prepare_retries == MAX_RETRIES)
        {
            log_event("Max retries reached for preparing SQL statement, skipping this data point.");
            continue;
        }

        if (sqlite3_bind_int(stmt, 1, data.sensor_id) != SQLITE_OK ||
            sqlite3_bind_double(stmt, 2, data.temperature) != SQLITE_OK ||
            sqlite3_bind_int64(stmt, 3, (sqlite3_int64)data.timestamp) != SQLITE_OK)
        {
            log_event("Failed to bind values to SQL statement");
            goto cleanup_stmt;
        }

        int step_retries = 0;
        while (step_retries < MAX_RETRIES && sqlite3_step(stmt) != SQLITE_DONE)
        {
            snprintf(msg, sizeof(msg), "Failed to insert row, retry %d/%d", step_retries + 1, MAX_RETRIES);
            log_event(msg);
            sleep(1);
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
    }

cleanup:
    if (stmt)
    {
        sqlite3_finalize(stmt);
    }
    if (db && sqlite3_close(db) != SQLITE_OK)
    {
        log_event("Failed to close database");
    }
    log_event("Storage manager shutting down");

    return NULL;
}
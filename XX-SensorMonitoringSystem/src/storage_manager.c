#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <unistd.h>
#include "storage_manager.h"
#include "common.h"

static const char* DB_PATH = "db/sensors.db";

void* storage_manager(void* arg)
{
    // open or create the database
    sqlite3* db;
    if (sqlite3_open(DB_PATH, &db) == SQLITE_OK)
    {
        log_event("Connection to SQL server established");
    }
    else
    {
        log_event("Failed to open database");
    }

    static const char* CREATE_TABLE = "CREATE TABLE IF NOT EXISTS measurements (id INTEGER, temp REAL, time INTEGER);";
    char* err_msg;
    if (sqlite3_exec(db, CREATE_TABLE, NULL, NULL, &err_msg) == SQLITE_OK)
    {
        log_event("New table measurements created");
    }
    else
    {
        printf("Failed to create table %s", err_msg);
        sqlite3_free(err_msg);
    }

    thread_args_t* args = (thread_args_t*)arg; 
    sbuffer_t* sb = args->sb;
    while (!shutdown_flag)
    {
        // Retrieve data
        sensor_data_t data;
        if (sbuffer_pop(sb, &data) != 0)
        {
            perror("Failed to pop data from sbuffer");
            log_event("Failed to pop data from sbuffer");
            continue;
        }

        // Insert Statement
        static const char* INSERT_STMT = "INSERT INTO measurements VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, INSERT_STMT, -1, &stmt, NULL) != SQLITE_OK)
        {
            int retries = 0;
            while (retries < 3)
            {
                if (sqlite3_prepare_v2(db, INSERT_STMT, -1, &stmt, NULL) == SQLITE_OK)
                {
                    break;
                }
                retries++;
            }
            // Signal the main process to shut down
            shutdown_flag = 1;
            return NULL;
            perror("Failed to prepare insert statement");
            log_event("Failed to prepare insert statement");
        }

        //Bind Values
        if (sqlite3_bind_int(stmt, 1, data.sensor_id) != SQLITE_OK)
        {
            perror("Failed to bind sensor id");
            log_event("Failed to bind sensor id");
        }

        if (sqlite3_bind_double(stmt, 2, data.temperature) != SQLITE_OK)
        {
            perror("Failed to bind temperature");
            log_event("Failed to bind temperature");
        }

        if (sqlite3_bind_int64(stmt, 3, (sqlite3_int64)data.timestamp) != SQLITE_OK)
        {
            perror("Failed to bind timestamp");
            log_event("Failed to bind timestamp");
        }

        // Execute statement
        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            int retries = 0;
            while (retries < 3)
            {
                if (sqlite3_step(stmt) == SQLITE_DONE)
                {
                    break;
                }
                retries++;
            }
            perror("Failed to insert row");
            log_event("Failed to insert row");
            if (sqlite3_close(db) != SQLITE_OK)
            {
                perror("Failed to close db");
                log_event("Failed to close db");
            }

            // Signal the main process to shut down
            shutdown_flag = 1;
            return NULL;
        }
        else
        {
            log_event("Inserted a row to sql table");
        }

        // Clean up statement
        sqlite3_finalize(stmt);
    }

    if (sqlite3_close(db) != SQLITE_OK)
    {
        perror("Failed to close db");
        log_event("Failed to close db");
    }

    return NULL;
}

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>

#include "crimpGc_internals.h"

static bool _crimpGc_console_logging_enabled = true;
static bool _crimpGc_file_logging_enabled = false;
static FILE* _crimpGc_file_logging;

static pthread_mutex_t _crimpGc_log_mutex = PTHREAD_MUTEX_INITIALIZER;

CRIMPGC_API void crimpGc_log(char const* func, char const* fmt, ...)
{
    int e;
    va_list args;
    va_start(args, fmt);

    e = pthread_mutex_lock(&_crimpGc_log_mutex); crimpGc_assert(!e);
    {
        if (_crimpGc_console_logging_enabled) {
            // since any va_list can only be traversed once, we make a copy for each time we try to use it
            va_list args_copy;
            va_copy(args_copy, args);

            if (crimpGc_appThread != NULL)
                fprintf(stderr, " (%02d) [%s] ", crimpGc_appThread->thread_id, func);
            else
                fprintf(stderr, " (\?\?) [%s] ", func);

            vfprintf(stderr, fmt, args_copy); 
            fprintf(stderr, "\n");

            va_end(args_copy);
        }

        if (_crimpGc_file_logging != NULL) {
            va_list args_copy;
            va_copy(args_copy, args);

            if (crimpGc_appThread != NULL)
                fprintf(_crimpGc_file_logging, " (%02d) [%s] ", crimpGc_appThread->thread_id, func);
            else
                fprintf(_crimpGc_file_logging, " (\?\?) [%s] ", func);

            vfprintf(_crimpGc_file_logging, fmt, args_copy);
            fprintf(_crimpGc_file_logging, "\n");
            fflush(_crimpGc_file_logging);

            va_end(args_copy);
        }
    }
    e = pthread_mutex_unlock(&_crimpGc_log_mutex);  crimpGc_assert(!e);

    va_end(args);
}

void crimpGc_logging_init() {
	if (_crimpGc_file_logging_enabled) {
		char filename[1024];
		sprintf(filename, "crimpGc_%lu.log", (unsigned long)time(NULL));
		_crimpGc_file_logging = fopen(filename, "w");
	}
}
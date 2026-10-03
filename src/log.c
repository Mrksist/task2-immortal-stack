#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>

#include "mystack.h"
#include "internal.h"

const char* const log_levels[] = {
    "DEBUG",
    "WARNING",
    "ERROR",
    "CRITICAL"
};

const char* const time_format = "%Y-%m-%d %H:%M:%S";

static FILE* log_file = 0;


static struct tm* current_time () {
    time_t time_t_now = time(NULL);
    struct tm *now = localtime(&time_t_now);

    return now;
}

__attribute__((constructor)) static void log_fd_open () {
    assert (log_file == 0);

    char fname[TMP_BUFFER_SIZE] = "stack.log";

    log_file = fopen (fname, "a");

    setbuf (log_file, 0);

    fprintf (log_file, "\n");
    fprintf (log_file, "========================= ImmortalStack started. %s. =========================\n", ST_MODE);
}

__attribute__((destructor)) static void log_fd_close () {
    fclose (log_file);
}

void LogWrite (int level, const char* format, const char* function, stack_t* stack, int nargs, ...) {
    assert (0 <= level && level <= 3);
    assert (format != 0);

    if (level < ST_LOG_LEVEL) {
        return;
    }

    struct tm *now = current_time();
    char time_buffer[TMP_BUFFER_SIZE] = {};
    strftime(time_buffer, sizeof(time_buffer), time_format, now);

    char message_buffer[MESSAGE_BUFFER_SIZE] = {};
    va_list va;
    va_start (va, nargs);
    vsprintf (message_buffer, format, va);
    va_end (va);

    fprintf (log_file, "[ %s -  %s ] \"%s (Stack at %llx): %s\"\n", time_buffer,  log_levels[level], function, (unsigned long long)stack, message_buffer);
}
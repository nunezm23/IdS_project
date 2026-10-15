#include "stdio_wrapper.h"

#if defined(TEST_STDIO_WRAPPER)

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MAX_CAPTURED_STDIO_CALLS 8U
#define MAX_CAPTURED_STDIO_TEXT 128U

static int observation_active;
static int fail_open;
static const char *target_path;
static const char *target_write;
static FILE *target_stream;
static int target_stream_open;
static size_t open_count;
static size_t write_count;
static size_t close_count;
static size_t printf_count;
static size_t unexpected_count;
static char captured_writes[MAX_CAPTURED_STDIO_CALLS][MAX_CAPTURED_STDIO_TEXT];
static char captured_printf[MAX_CAPTURED_STDIO_CALLS][MAX_CAPTURED_STDIO_TEXT];
static char unexpected_operations[MAX_CAPTURED_STDIO_CALLS][32];
static char unexpected_details[MAX_CAPTURED_STDIO_CALLS][MAX_CAPTURED_STDIO_TEXT];
static test_stdio_wrapper_event_callback_t event_callback;

FILE *__real_fopen(const char *path, const char *mode);
int __real_fputs(const char *text, FILE *stream);
int __real_fclose(FILE *stream);

static void record_unexpected(const char *operation, const char *detail)
{
    if (unexpected_count < MAX_CAPTURED_STDIO_CALLS) {
        (void)snprintf(unexpected_operations[unexpected_count],
                       sizeof(unexpected_operations[unexpected_count]),
                       "%s",
                       operation);
        (void)snprintf(unexpected_details[unexpected_count],
                       sizeof(unexpected_details[unexpected_count]),
                       "%s",
                       detail == NULL ? "" : detail);
    }
    unexpected_count++;
    if (event_callback != NULL) {
        event_callback(operation, detail);
    }
}

static int is_snapshot_format(const char *format)
{
    return format != NULL &&
           (strcmp(format, "ID        = 0x%02X\n") == 0 ||
           strcmp(format, "POWER     = 0x%02X\n") == 0 ||
           strcmp(format, "INTERFACE = 0x%02X\n") == 0 ||
           strcmp(format, "MODE0     = 0x%02X\n") == 0 ||
           strcmp(format, "MODE1     = 0x%02X\n") == 0);
}

void test_stdio_wrapper_reset(void)
{
    observation_active = 0;
    fail_open = 0;
    target_path = NULL;
    target_write = NULL;
    target_stream = NULL;
    target_stream_open = 0;
    open_count = 0U;
    write_count = 0U;
    close_count = 0U;
    printf_count = 0U;
    unexpected_count = 0U;
    memset(captured_writes, 0, sizeof(captured_writes));
    memset(captured_printf, 0, sizeof(captured_printf));
    memset(unexpected_operations, 0, sizeof(unexpected_operations));
    memset(unexpected_details, 0, sizeof(unexpected_details));
    event_callback = NULL;
}

void test_stdio_wrapper_begin(const char *observed_path,
                              const char *expected_write,
                              int force_open_failure,
                              test_stdio_wrapper_event_callback_t callback)
{
    target_path = observed_path;
    target_write = expected_write;
    fail_open = force_open_failure;
    event_callback = callback;
    observation_active = 1;
}

void test_stdio_wrapper_end(void)
{
    observation_active = 0;
    event_callback = NULL;
}

size_t test_stdio_wrapper_open_count(void)
{
    return open_count;
}

size_t test_stdio_wrapper_write_count(void)
{
    return write_count;
}

size_t test_stdio_wrapper_close_count(void)
{
    return close_count;
}

size_t test_stdio_wrapper_printf_count(void)
{
    return printf_count;
}

size_t test_stdio_wrapper_unexpected_count(void)
{
    return unexpected_count;
}

const char *test_stdio_wrapper_write(size_t call_index)
{
    if (call_index >= write_count || call_index >= MAX_CAPTURED_STDIO_CALLS) {
        return NULL;
    }
    return captured_writes[call_index];
}

const char *test_stdio_wrapper_printf_output(size_t call_index)
{
    if (call_index >= printf_count || call_index >= MAX_CAPTURED_STDIO_CALLS) {
        return NULL;
    }
    return captured_printf[call_index];
}

const char *test_stdio_wrapper_unexpected_operation(size_t call_index)
{
    if (call_index >= unexpected_count ||
        call_index >= MAX_CAPTURED_STDIO_CALLS) {
        return NULL;
    }
    return unexpected_operations[call_index];
}

const char *test_stdio_wrapper_unexpected_detail(size_t call_index)
{
    if (call_index >= unexpected_count ||
        call_index >= MAX_CAPTURED_STDIO_CALLS) {
        return NULL;
    }
    return unexpected_details[call_index];
}

FILE *test_stdio_wrapper_stream(void)
{
    return target_stream;
}

FILE *__wrap_fopen(const char *path, const char *mode)
{
    if (observation_active == 0) {
        return __real_fopen(path, mode);
    }

    if (path == NULL || mode == NULL || target_path == NULL ||
        strcmp(path, target_path) != 0 || strcmp(mode, "w") != 0 ||
        open_count != 0U) {
        record_unexpected("unexpected_fopen", path);
        errno = EPERM;
        return NULL;
    }

    open_count++;
    if (event_callback != NULL) {
        event_callback("fopen", path);
    }
    if (fail_open != 0) {
        errno = EACCES;
        target_stream = NULL;
        return NULL;
    }

    target_stream = __real_fopen(path, mode);
    target_stream_open = target_stream != NULL;
    return target_stream;
}

int __wrap_fputs(const char *text, FILE *stream)
{
    if (observation_active == 0) {
        return __real_fputs(text, stream);
    }

    if (text == NULL || target_write == NULL ||
        strcmp(text, target_write) != 0 || stream == NULL ||
        stream != target_stream || target_stream_open == 0 ||
        write_count != 0U) {
        record_unexpected("unexpected_fputs", text);
        errno = EPERM;
        return EOF;
    }

    if (write_count < MAX_CAPTURED_STDIO_CALLS) {
        (void)snprintf(captured_writes[write_count],
                       sizeof(captured_writes[write_count]),
                       "%s",
                       text);
    }
    write_count++;
    if (event_callback != NULL) {
        event_callback("fputs", text);
    }
    return __real_fputs(text, stream);
}

int __wrap_fclose(FILE *stream)
{
    if (observation_active == 0) {
        return __real_fclose(stream);
    }

    if (stream == NULL || stream != target_stream ||
        target_stream_open == 0 || close_count != 0U) {
        record_unexpected("unexpected_fclose", NULL);
        errno = EPERM;
        return EOF;
    }

    close_count++;
    target_stream_open = 0;
    if (event_callback != NULL) {
        event_callback("fclose", NULL);
    }
    return __real_fclose(stream);
}

int __wrap_printf(const char *format, ...)
{
    va_list args;
    int result;

    va_start(args, format);
    if (observation_active != 0 && is_snapshot_format(format)) {
        char *output = NULL;

        if (printf_count < MAX_CAPTURED_STDIO_CALLS) {
            output = captured_printf[printf_count];
            result = vsnprintf(output,
                               sizeof(captured_printf[printf_count]),
                               format,
                               args);
        } else {
            char discarded[MAX_CAPTURED_STDIO_TEXT];
            result = vsnprintf(discarded, sizeof(discarded), format, args);
        }
        printf_count++;
        if (event_callback != NULL) {
            event_callback("printf", output);
        }
    } else if (observation_active == 0) {
        result = vprintf(format, args);
    } else {
        record_unexpected("unexpected_printf", format);
        errno = EPERM;
        result = -1;
    }
    va_end(args);
    return result;
}

#endif

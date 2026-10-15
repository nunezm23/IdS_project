#ifndef TEST_STDIO_WRAPPER_H
#define TEST_STDIO_WRAPPER_H

#include <stddef.h>
#include <stdio.h>

typedef void (*test_stdio_wrapper_event_callback_t)(const char *operation,
                                                    const char *detail);

void test_stdio_wrapper_reset(void);
void test_stdio_wrapper_begin(const char *observed_path,
                              const char *expected_write,
                              int force_open_failure,
                              test_stdio_wrapper_event_callback_t callback);
void test_stdio_wrapper_end(void);
size_t test_stdio_wrapper_open_count(void);
size_t test_stdio_wrapper_write_count(void);
size_t test_stdio_wrapper_close_count(void);
size_t test_stdio_wrapper_printf_count(void);
size_t test_stdio_wrapper_unexpected_count(void);
const char *test_stdio_wrapper_write(size_t call_index);
const char *test_stdio_wrapper_printf_output(size_t call_index);
const char *test_stdio_wrapper_unexpected_operation(size_t call_index);
const char *test_stdio_wrapper_unexpected_detail(size_t call_index);
FILE *test_stdio_wrapper_stream(void);

#endif /* TEST_STDIO_WRAPPER_H */

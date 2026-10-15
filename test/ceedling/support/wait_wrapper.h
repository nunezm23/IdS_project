#ifndef TEST_WAIT_WRAPPER_H
#define TEST_WAIT_WRAPPER_H

#include <stddef.h>
#include <time.h>

typedef void (*test_wait_wrapper_event_callback_t)(
    const struct timespec *duration,
    size_t call_index);

void test_wait_wrapper_reset(void);
size_t test_wait_wrapper_call_count(void);
void test_wait_wrapper_fail_then_succeed(size_t failures, int error_number);
void test_wait_wrapper_fail_once_with_remainder(
    int error_number,
    const struct timespec *remainder);
void test_wait_wrapper_set_event_callback(
    test_wait_wrapper_event_callback_t callback);
struct timespec test_wait_wrapper_duration(size_t call_index);
int test_wait_wrapper_real_nanosleep(const struct timespec *duration,
                                     struct timespec *remainder);

#endif /* TEST_WAIT_WRAPPER_H */

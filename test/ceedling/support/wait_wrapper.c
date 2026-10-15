#include "wait_wrapper.h"

#include <errno.h>
#include <time.h>

#define MAX_CAPTURED_WAITS 1000U

static size_t wait_call_count;
static size_t failures_before_success;
static int failure_errno;
static struct timespec captured_durations[MAX_CAPTURED_WAITS];

int __real_nanosleep(const struct timespec *duration, struct timespec *remainder);

void test_wait_wrapper_reset(void)
{
    wait_call_count = 0U;
    failures_before_success = 0U;
    failure_errno = 0;
}

size_t test_wait_wrapper_call_count(void)
{
    return wait_call_count;
}

void test_wait_wrapper_fail_then_succeed(size_t failures, int error_number)
{
    failures_before_success = failures;
    failure_errno = error_number;
}

struct timespec test_wait_wrapper_duration(size_t call_index)
{
    struct timespec zero = { 0, 0 };

    if (call_index >= wait_call_count || call_index >= MAX_CAPTURED_WAITS) {
        return zero;
    }

    return captured_durations[call_index];
}

int test_wait_wrapper_real_nanosleep(const struct timespec *duration,
                                     struct timespec *remainder)
{
    return __real_nanosleep(duration, remainder);
}

int __wrap_nanosleep(const struct timespec *duration, struct timespec *remainder)
{
    (void)remainder;

    if (duration != NULL && wait_call_count < MAX_CAPTURED_WAITS) {
        captured_durations[wait_call_count] = *duration;
    }
    wait_call_count++;

    if (wait_call_count <= failures_before_success) {
        errno = failure_errno;
        return -1;
    }

    return 0;
}

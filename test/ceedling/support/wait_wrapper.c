#include "wait_wrapper.h"

#include <time.h>

static size_t wait_call_count;

void test_wait_wrapper_reset(void)
{
    wait_call_count = 0U;
}

size_t test_wait_wrapper_call_count(void)
{
    return wait_call_count;
}

int __wrap_nanosleep(const struct timespec *duration, struct timespec *remainder)
{
    (void)duration;
    (void)remainder;
    wait_call_count++;
    return 0;
}

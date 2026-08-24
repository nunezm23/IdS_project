#include "app_log.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

static void log_with_level(FILE *stream, const char *level, const char *fmt, va_list args)
{
    fprintf(stream, "[%s] ", level);
    vfprintf(stream, fmt, args);
    fputc('\n', stream);
}

void app_log_info(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log_with_level(stdout, "INFO", fmt, args);
    va_end(args);
}

void app_log_warn(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log_with_level(stdout, "WARN", fmt, args);
    va_end(args);
}

void app_log_error(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    log_with_level(stderr, "ERROR", fmt, args);
    va_end(args);
}

void app_log_sample(uint32_t sample_index, const ads1263_sample_t *sample)
{
    if (sample == NULL) {
        return;
    }

    printf("sample=%03" PRIu32 " raw=%011" PRId32 " voltage=%.6f drdy=%d status=%s\n",
           sample_index,
           sample->raw,
           sample->voltage,
           sample->drdy,
           sample->status != NULL ? sample->status : "UNKNOWN");
}

void app_log_csv_sample(FILE *stream, uint32_t sample_index, const ads1263_sample_t *sample)
{
    if (stream == NULL || sample == NULL) {
        return;
    }

    fprintf(stream,
            "%" PRIu32 ",%" PRId32 ",%.9f,%s\n",
            sample_index,
            sample->raw,
            sample->voltage,
            sample->status != NULL ? sample->status : "UNKNOWN");
}

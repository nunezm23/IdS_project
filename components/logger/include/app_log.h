#ifndef APP_LOG_H
#define APP_LOG_H

#include "ads1263_defs.h"

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Print an informational message.
 */
void app_log_info(const char *fmt, ...);

/**
 * @brief Print a warning message.
 */
void app_log_warn(const char *fmt, ...);

/**
 * @brief Print an error message.
 */
void app_log_error(const char *fmt, ...);

/**
 * @brief Print one ADC sample using the demo console format.
 */
void app_log_sample(uint32_t sample_index, const ads1263_sample_t *sample);

/**
 * @brief Write one ADC sample to a CSV stream.
 */
void app_log_csv_sample(FILE *stream, uint32_t sample_index, const ads1263_sample_t *sample);

#endif /* APP_LOG_H */

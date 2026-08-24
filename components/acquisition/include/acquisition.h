#ifndef ACQUISITION_H
#define ACQUISITION_H

#include "ads1263_defs.h"

#include <stdint.h>

typedef struct {
    uint8_t channel;
    uint32_t samples;
    uint32_t rate_hz;
    int live;
    const char *csv_path;
} acquisition_config_t;

/**
 * @brief Return default demo acquisition parameters.
 */
acquisition_config_t acquisition_default_config(void);

/**
 * @brief Run ADS1263 register/DRDY validation.
 */
int acquisition_run_check(const ads1263_config_t *adc_config);

/**
 * @brief Acquire a fixed number of ADC samples.
 */
int acquisition_run_samples(const ads1263_config_t *adc_config,
                            const acquisition_config_t *acq_config);

/**
 * @brief Acquire ADC samples until SIGINT.
 */
int acquisition_run_live(const ads1263_config_t *adc_config,
                         const acquisition_config_t *acq_config);

#endif /* ACQUISITION_H */

#include "unity.h"

#include "acquisition.h"
#include "mock_ads1263.h"
#include "wait_wrapper.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static unsigned int info_calls;
static unsigned int warning_calls;
static unsigned int error_calls;
static unsigned int sample_calls;
static unsigned int csv_sample_calls;
static const char *last_sample_status;
static int last_sample_drdy;
static int32_t last_sample_raw;
static ads1263_register_snapshot_t returned_snapshot;
static ads1263_sample_t returned_sample;

void app_log_info(const char *fmt, ...)
{
    (void)fmt;
    info_calls++;
}

void app_log_warn(const char *fmt, ...)
{
    (void)fmt;
    warning_calls++;
}

void app_log_error(const char *fmt, ...)
{
    (void)fmt;
    error_calls++;
}

void app_log_sample(uint32_t sample_index, const ads1263_sample_t *sample)
{
    TEST_ASSERT_EQUAL_UINT32(sample_calls + 1U, sample_index);
    TEST_ASSERT_NOT_NULL(sample);
    sample_calls++;
    last_sample_status = sample->status;
    last_sample_drdy = sample->drdy;
    last_sample_raw = sample->raw;
}

void app_log_csv_sample(FILE *stream,
                        uint32_t sample_index,
                        const ads1263_sample_t *sample)
{
    TEST_ASSERT_NULL(stream);
    TEST_ASSERT_EQUAL_UINT32(csv_sample_calls + 1U, sample_index);
    TEST_ASSERT_NOT_NULL(sample);
    csv_sample_calls++;
}

static ads1263_config_t make_adc_config(void)
{
    ads1263_config_t config;

    memset(&config, 0, sizeof(config));
    config.spi_device = "/dev/fake-spi";
    config.gpio_chip = "fakechip";
    config.drdy_line = 8U;
    config.spi_speed_hz = 1000000U;
    config.vref = 2.5;
    config.gain = 1U;
    config.use_drdy = 1;
    return config;
}

static acquisition_config_t make_acquisition_config(uint32_t samples,
                                                    uint32_t rate_hz)
{
    acquisition_config_t config;

    memset(&config, 0, sizeof(config));
    config.samples = samples;
    config.rate_hz = rate_hz;
    return config;
}

static void expect_preparation_success(void)
{
    returned_snapshot.id = 0x23U;
    returned_snapshot.power = 0x11U;
    returned_snapshot.interface = 0x05U;
    returned_snapshot.mode0 = 0x00U;
    returned_snapshot.mode1 = 0x80U;

    ads1263_init_ExpectAndReturn(NULL, NULL, ADS1263_OK);
    ads1263_init_IgnoreArg_dev();
    ads1263_init_IgnoreArg_config();
    ads1263_check_presence_ExpectAndReturn(NULL, NULL, ADS1263_OK);
    ads1263_check_presence_IgnoreArg_dev();
    ads1263_check_presence_IgnoreArg_snapshot();
    ads1263_check_presence_ReturnThruPtr_snapshot(&returned_snapshot);
    ads1263_configure_channel_ExpectAndReturn(NULL, 0U, ADS1263_OK);
    ads1263_configure_channel_IgnoreArg_dev();
}

static void expect_sample(const char *status,
                          int drdy,
                          ads1263_status_t result)
{
    memset(&returned_sample, 0, sizeof(returned_sample));
    returned_sample.raw = 42;
    returned_sample.voltage = 4.8894435190080868e-08;
    returned_sample.drdy = drdy;
    returned_sample.status_byte = 0xA5U;
    returned_sample.checksum_byte = 0x3CU;
    returned_sample.status = status;

    ads1263_read_sample_ExpectAndReturn(NULL, NULL, result);
    ads1263_read_sample_IgnoreArg_dev();
    ads1263_read_sample_IgnoreArg_sample();
    ads1263_read_sample_ReturnThruPtr_sample(&returned_sample);
}

void setUp(void)
{
    info_calls = 0U;
    warning_calls = 0U;
    error_calls = 0U;
    sample_calls = 0U;
    csv_sample_calls = 0U;
    last_sample_status = NULL;
    last_sample_drdy = 0;
    last_sample_raw = 0;
    memset(&returned_snapshot, 0, sizeof(returned_snapshot));
    memset(&returned_sample, 0, sizeof(returned_sample));
    test_wait_wrapper_reset();
}

void tearDown(void)
{
}

void test_UT_ACQ_ROB003_001_gpio_status_sample_is_emitted_and_run_succeeds(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U);

    expect_preparation_success();
    expect_sample("GPIO_READ", -1, ADS1263_OK);
    ads1263_close_Expect(NULL);
    ads1263_close_IgnoreArg_dev();

    TEST_ASSERT_EQUAL_INT(0, acquisition_run_samples(&adc, &acq));
    TEST_ASSERT_EQUAL_UINT(1U, sample_calls);
    TEST_ASSERT_EQUAL_UINT(1U, csv_sample_calls);
    TEST_ASSERT_EQUAL_STRING("GPIO_READ", last_sample_status);
    TEST_ASSERT_EQUAL_INT(-1, last_sample_drdy);
    TEST_ASSERT_EQUAL_INT32(42, last_sample_raw);
    TEST_ASSERT_EQUAL_UINT(0U, error_calls);
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_ACQ_ROB003_002_timeout_status_sample_is_emitted_and_run_succeeds(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U);

    expect_preparation_success();
    expect_sample("DRDY_TIMEOUT", 1, ADS1263_OK);
    ads1263_close_Expect(NULL);
    ads1263_close_IgnoreArg_dev();

    TEST_ASSERT_EQUAL_INT(0, acquisition_run_samples(&adc, &acq));
    TEST_ASSERT_EQUAL_UINT(1U, sample_calls);
    TEST_ASSERT_EQUAL_UINT(1U, csv_sample_calls);
    TEST_ASSERT_EQUAL_STRING("DRDY_TIMEOUT", last_sample_status);
    TEST_ASSERT_EQUAL_INT(1, last_sample_drdy);
    TEST_ASSERT_EQUAL_INT32(42, last_sample_raw);
    TEST_ASSERT_EQUAL_UINT(0U, error_calls);
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_ACQ_ROB003_003_gpio_then_spi_failure_aborts_without_output(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U);

    expect_preparation_success();
    expect_sample("GPIO_READ", -1, ADS1263_ERROR_SPI_TRANSFER);
    ads1263_status_to_string_ExpectAndReturn(ADS1263_ERROR_SPI_TRANSFER,
                                             "SPI_TRANSFER");
    ads1263_close_Expect(NULL);
    ads1263_close_IgnoreArg_dev();

    TEST_ASSERT_EQUAL_INT(-1, acquisition_run_samples(&adc, &acq));
    TEST_ASSERT_EQUAL_UINT(0U, sample_calls);
    TEST_ASSERT_EQUAL_UINT(0U, csv_sample_calls);
    TEST_ASSERT_EQUAL_UINT(1U, error_calls);
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_ACQ_ROB003_004_timeout_then_spi_failure_aborts_without_output(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U);

    expect_preparation_success();
    expect_sample("DRDY_TIMEOUT", 1, ADS1263_ERROR_SPI_TRANSFER);
    ads1263_status_to_string_ExpectAndReturn(ADS1263_ERROR_SPI_TRANSFER,
                                             "SPI_TRANSFER");
    ads1263_close_Expect(NULL);
    ads1263_close_IgnoreArg_dev();

    TEST_ASSERT_EQUAL_INT(-1, acquisition_run_samples(&adc, &acq));
    TEST_ASSERT_EQUAL_UINT(0U, sample_calls);
    TEST_ASSERT_EQUAL_UINT(0U, csv_sample_calls);
    TEST_ASSERT_EQUAL_UINT(1U, error_calls);
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_ACQ_DSP017_001_rate_one_builds_invalid_timespec_on_every_retry(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(2U, 1U);

    expect_preparation_success();
    expect_sample("OK", 0, ADS1263_OK);
    expect_sample("OK", 0, ADS1263_OK);
    ads1263_close_Expect(NULL);
    ads1263_close_IgnoreArg_dev();
    test_wait_wrapper_fail_then_succeed(2U, EINVAL);

    TEST_ASSERT_EQUAL_INT(0, acquisition_run_samples(&adc, &acq));
    TEST_ASSERT_EQUAL_UINT(2U, sample_calls);
    TEST_ASSERT_EQUAL_size_t(3U, test_wait_wrapper_call_count());
    for (size_t i = 0U; i < test_wait_wrapper_call_count(); ++i) {
        struct timespec duration = test_wait_wrapper_duration(i);
        TEST_ASSERT_EQUAL_INT64(0, duration.tv_sec);
        TEST_ASSERT_EQUAL_INT64(1000000000L, duration.tv_nsec);
    }
}

void test_UT_ACQ_DSP017_002_host_posix_rejects_captured_timespec(void)
{
    const struct timespec invalid = { 0, 1000000000L };

    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, test_wait_wrapper_real_nanosleep(&invalid, NULL));
    TEST_ASSERT_EQUAL_INT(EINVAL, errno);
}

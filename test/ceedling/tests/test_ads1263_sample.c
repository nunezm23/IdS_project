#include "unity.h"

#include "ads1263.h"
#include "mock_ads1263_gpio_linux.h"
#include "mock_ads1263_spi_linux.h"
#include "wait_wrapper.h"

#include <limits.h>
#include <string.h>

#define VOLTAGE_TOLERANCE 1e-12
#define DRDY_TIMEOUT_POLLS 1000

static ads1263_t *expected_device;
static uint8_t injected_sample[7];
static ads1263_status_t spi_result;
static int spi_calls;
static int expected_gpio_before_spi;
static size_t expected_waits_before_spi;

static int gpio_values[3];
static ads1263_status_t gpio_results[3];
static int gpio_step_count;
static int gpio_calls;
static int gpio_timeout_mode;

static ads1263_t make_device(int mock, int use_drdy)
{
    ads1263_t dev;

    memset(&dev, 0, sizeof(dev));
    dev.config.mock = mock;
    dev.config.use_drdy = use_drdy;
    dev.config.vref = 2.5;
    dev.config.gain = 1U;
    return dev;
}

static ads1263_status_t sample_transfer_callback(ads1263_spi_linux_t *spi,
                                                 const uint8_t *tx,
                                                 uint8_t *rx,
                                                 size_t len,
                                                 int call_index)
{
    const uint8_t expected_tx[7] = { 0x12U, 0U, 0U, 0U, 0U, 0U, 0U };

    TEST_ASSERT_EQUAL_PTR(&expected_device->spi, spi);
    TEST_ASSERT_EQUAL_INT(spi_calls, call_index);
    TEST_ASSERT_EQUAL_size_t(sizeof(expected_tx), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_tx, tx, sizeof(expected_tx));
    TEST_ASSERT_EQUAL_INT(expected_gpio_before_spi, gpio_calls);
    TEST_ASSERT_EQUAL_size_t(expected_waits_before_spi,
                             test_wait_wrapper_call_count());
    memcpy(rx, injected_sample, sizeof(injected_sample));
    spi_calls++;
    return spi_result;
}

static ads1263_status_t gpio_callback(ads1263_gpio_linux_t *gpio,
                                      int *value,
                                      int call_index)
{
    TEST_ASSERT_EQUAL_PTR(&expected_device->gpio, gpio);
    TEST_ASSERT_EQUAL_INT(gpio_calls, call_index);

    if (gpio_timeout_mode != 0) {
        TEST_ASSERT_LESS_THAN_INT(DRDY_TIMEOUT_POLLS, call_index);
        *value = 1;
        gpio_calls++;
        return ADS1263_OK;
    }

    TEST_ASSERT_LESS_THAN_INT(gpio_step_count, call_index);
    if (gpio_results[call_index] == ADS1263_OK) {
        *value = gpio_values[call_index];
    }
    gpio_calls++;
    return gpio_results[call_index];
}

static void expect_sample_transfer(ads1263_t *dev,
                                   const uint8_t rx[7],
                                   ads1263_status_t result,
                                   int gpio_calls_before_spi,
                                   size_t waits_before_spi)
{
    expected_device = dev;
    memcpy(injected_sample, rx, sizeof(injected_sample));
    spi_result = result;
    expected_gpio_before_spi = gpio_calls_before_spi;
    expected_waits_before_spi = waits_before_spi;
    ads1263_spi_linux_transfer_StubWithCallback(sample_transfer_callback);
}

static void expect_gpio_sequence(ads1263_t *dev,
                                 const int *values,
                                 const ads1263_status_t *results,
                                 int count)
{
    expected_device = dev;
    gpio_step_count = count;
    for (int i = 0; i < count; ++i) {
        gpio_values[i] = values[i];
        gpio_results[i] = results[i];
    }
    ads1263_gpio_linux_read_drdy_StubWithCallback(gpio_callback);
}

static void expect_gpio_timeout(ads1263_t *dev)
{
    expected_device = dev;
    gpio_timeout_mode = 1;
    ads1263_gpio_linux_read_drdy_StubWithCallback(gpio_callback);
}

static void assert_sample(const ads1263_sample_t *sample,
                          int32_t raw,
                          double voltage,
                          int drdy,
                          uint8_t status_byte,
                          uint8_t checksum_byte,
                          const char *status)
{
    TEST_ASSERT_EQUAL_INT32(raw, sample->raw);
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, voltage, sample->voltage);
    TEST_ASSERT_EQUAL_INT(drdy, sample->drdy);
    TEST_ASSERT_EQUAL_HEX8(status_byte, sample->status_byte);
    TEST_ASSERT_EQUAL_HEX8(checksum_byte, sample->checksum_byte);
    TEST_ASSERT_EQUAL_STRING(status, sample->status);
}

void setUp(void)
{
    expected_device = NULL;
    memset(injected_sample, 0, sizeof(injected_sample));
    spi_result = ADS1263_OK;
    spi_calls = 0;
    expected_gpio_before_spi = 0;
    expected_waits_before_spi = 0U;
    memset(gpio_values, 0, sizeof(gpio_values));
    memset(gpio_results, 0, sizeof(gpio_results));
    gpio_step_count = 0;
    gpio_calls = 0;
    gpio_timeout_mode = 0;
    test_wait_wrapper_reset();
}

void tearDown(void)
{
}

void test_UT_SAMPLE_001_A_rejects_null_device(void)
{
    ads1263_sample_t sample;

    memset(&sample, 0xA5, sizeof(sample));
    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_sample(NULL, &sample));
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_SAMPLE_001_B_rejects_null_sample(void)
{
    ads1263_t dev = make_device(0, 1);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_sample(&dev, NULL));
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_SAMPLE_002_mock_samples_are_deterministic(void)
{
    ads1263_t dev = make_device(1, 1);
    ads1263_sample_t first;
    ads1263_sample_t second;

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &first));
    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &second));

    assert_sample(&first, 1234567, 0.0014372251468883946,
                  1, 0x80U, 0x00U, "OK");
    assert_sample(&second, 2469134, 0.0028744502937767891,
                  1, 0x80U, 0x00U, "OK");
    TEST_ASSERT_EQUAL_UINT32(2U, dev.mock_sample_counter);
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_SAMPLE_003_decodes_positive_value_and_exact_rdata1_frame(void)
{
    ads1263_t dev = make_device(0, 0);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0x00U, 0x80U, 0x00U, 0x00U,
                            0x00U, 0x01U, 0x5AU };

    expect_sample_transfer(&dev, rx, ADS1263_OK, 0, 0U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, 1, 1.1641532188114492e-09,
                  -1, 0x80U, 0x5AU, "OK");
    TEST_ASSERT_EQUAL_INT(1, spi_calls);
}

void test_UT_SAMPLE_004_decodes_zero_value(void)
{
    ads1263_t dev = make_device(0, 0);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0x00U, 0x80U, 0x00U, 0x00U,
                            0x00U, 0x00U, 0x5AU };

    expect_sample_transfer(&dev, rx, ADS1263_OK, 0, 0U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, 0, 0.0, -1, 0x80U, 0x5AU, "OK");
}

void test_UT_SAMPLE_005_decodes_positive_full_scale(void)
{
    ads1263_t dev = make_device(0, 0);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0x00U, 0x80U, 0x7FU, 0xFFU,
                            0xFFU, 0xFFU, 0x5AU };

    expect_sample_transfer(&dev, rx, ADS1263_OK, 0, 0U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, INT32_MAX, 2.5, -1, 0x80U, 0x5AU, "OK");
}

void test_UT_SAMPLE_006_decodes_negative_full_scale(void)
{
    ads1263_t dev = make_device(0, 0);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0x00U, 0x80U, 0x80U, 0x00U,
                            0x00U, 0x00U, 0x5AU };

    expect_sample_transfer(&dev, rx, ADS1263_OK, 0, 0U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, INT32_MIN, -2.5000000011641532,
                  -1, 0x80U, 0x5AU, "OK");
}

void test_UT_SAMPLE_007_extracts_status_checksum_and_voltage(void)
{
    ads1263_t dev = make_device(0, 0);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0x00U, 0xA5U, 0x00U, 0x00U,
                            0x00U, 0x2AU, 0x3CU };

    expect_sample_transfer(&dev, rx, ADS1263_OK, 0, 0U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, 42, 4.8894435190080868e-08,
                  -1, 0xA5U, 0x3CU, "OK");
}

void test_UT_SAMPLE_008_propagates_spi_failure_and_leaves_zeroed_sample(void)
{
    ads1263_t dev = make_device(0, 0);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0U };

    memset(&sample, 0xA5, sizeof(sample));
    expect_sample_transfer(&dev, rx, ADS1263_ERROR_SPI_TRANSFER, 0, 0U);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_read_sample(&dev, &sample));
    TEST_ASSERT_EQUAL_INT32(0, sample.raw);
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, 0.0, sample.voltage);
    TEST_ASSERT_EQUAL_INT(0, sample.drdy);
    TEST_ASSERT_EQUAL_HEX8(0U, sample.status_byte);
    TEST_ASSERT_EQUAL_HEX8(0U, sample.checksum_byte);
    TEST_ASSERT_NULL(sample.status);
    TEST_ASSERT_EQUAL_INT(1, spi_calls);
}

void test_UT_DRDY_001_A_read_drdy_rejects_null_device(void)
{
    int value = 7;

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_drdy(NULL, &value));
    TEST_ASSERT_EQUAL_INT(7, value);
}

void test_UT_DRDY_001_B_read_drdy_rejects_null_output(void)
{
    ads1263_t dev = make_device(0, 1);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_drdy(&dev, NULL));
}

void test_UT_DRDY_002_immediate_ready_reads_without_waiting(void)
{
    ads1263_t dev = make_device(0, 1);
    ads1263_sample_t sample;
    const int values[1] = { 0 };
    const ads1263_status_t results[1] = { ADS1263_OK };
    const uint8_t rx[7] = { 0x00U, 0xA5U, 0x00U, 0x00U,
                            0x00U, 0x2AU, 0x3CU };

    expect_gpio_sequence(&dev, values, results, 1);
    expect_sample_transfer(&dev, rx, ADS1263_OK, 1, 0U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, 42, 4.8894435190080868e-08,
                  0, 0xA5U, 0x3CU, "OK");
    TEST_ASSERT_EQUAL_INT(1, gpio_calls);
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_DRDY_003_delayed_ready_polls_three_times_and_waits_twice(void)
{
    ads1263_t dev = make_device(0, 1);
    ads1263_sample_t sample;
    const int values[3] = { 1, 1, 0 };
    const ads1263_status_t results[3] = {
        ADS1263_OK, ADS1263_OK, ADS1263_OK
    };
    const uint8_t rx[7] = { 0x00U, 0xA5U, 0x00U, 0x00U,
                            0x00U, 0x2AU, 0x3CU };

    expect_gpio_sequence(&dev, values, results, 3);
    expect_sample_transfer(&dev, rx, ADS1263_OK, 3, 2U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, 42, 4.8894435190080868e-08,
                  0, 0xA5U, 0x3CU, "OK");
    TEST_ASSERT_EQUAL_INT(3, gpio_calls);
    TEST_ASSERT_EQUAL_size_t(2U, test_wait_wrapper_call_count());
}

void test_UT_DRDY_004_gpio_error_is_recorded_while_spi_success_returns_ok(void)
{
    ads1263_t dev = make_device(0, 1);
    ads1263_sample_t sample;
    const int values[1] = { 0 };
    const ads1263_status_t results[1] = { ADS1263_ERROR_GPIO_READ };
    const uint8_t rx[7] = { 0x00U, 0xA5U, 0x00U, 0x00U,
                            0x00U, 0x2AU, 0x3CU };

    expect_gpio_sequence(&dev, values, results, 1);
    expect_sample_transfer(&dev, rx, ADS1263_OK, 1, 0U);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, 42, 4.8894435190080868e-08,
                  -1, 0xA5U, 0x3CU, "GPIO_READ");
    TEST_ASSERT_EQUAL_INT(1, gpio_calls);
    TEST_ASSERT_EQUAL_INT(1, spi_calls);
}

void test_UT_DRDY_005_timeout_is_bounded_and_recorded_after_spi_success(void)
{
    ads1263_t dev = make_device(0, 1);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0x00U, 0xA5U, 0x00U, 0x00U,
                            0x00U, 0x2AU, 0x3CU };

    expect_gpio_timeout(&dev);
    expect_sample_transfer(&dev, rx, ADS1263_OK,
                           DRDY_TIMEOUT_POLLS, DRDY_TIMEOUT_POLLS);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_sample(&dev, &sample));
    assert_sample(&sample, 42, 4.8894435190080868e-08,
                  1, 0xA5U, 0x3CU, "DRDY_TIMEOUT");
    TEST_ASSERT_EQUAL_INT(DRDY_TIMEOUT_POLLS, gpio_calls);
    TEST_ASSERT_EQUAL_size_t(DRDY_TIMEOUT_POLLS,
                             test_wait_wrapper_call_count());
    TEST_ASSERT_EQUAL_INT(1, spi_calls);
}

void test_UT_DRDY_006_gpio_error_followed_by_spi_failure_returns_spi_error(void)
{
    ads1263_t dev = make_device(0, 1);
    ads1263_sample_t sample;
    const int values[1] = { 0 };
    const ads1263_status_t results[1] = { ADS1263_ERROR_GPIO_READ };
    const uint8_t rx[7] = { 0U };

    memset(&sample, 0xA5, sizeof(sample));
    expect_gpio_sequence(&dev, values, results, 1);
    expect_sample_transfer(&dev, rx, ADS1263_ERROR_SPI_TRANSFER, 1, 0U);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_read_sample(&dev, &sample));
    TEST_ASSERT_EQUAL_MEMORY((const uint8_t[sizeof(sample)]){ 0U },
                             &sample,
                             sizeof(sample));
    TEST_ASSERT_EQUAL_INT(1, gpio_calls);
    TEST_ASSERT_EQUAL_INT(1, spi_calls);
    TEST_ASSERT_EQUAL_size_t(0U, test_wait_wrapper_call_count());
}

void test_UT_DRDY_007_timeout_followed_by_spi_failure_returns_spi_error(void)
{
    ads1263_t dev = make_device(0, 1);
    ads1263_sample_t sample;
    const uint8_t rx[7] = { 0U };

    memset(&sample, 0xA5, sizeof(sample));
    expect_gpio_timeout(&dev);
    expect_sample_transfer(&dev, rx, ADS1263_ERROR_SPI_TRANSFER,
                           DRDY_TIMEOUT_POLLS, DRDY_TIMEOUT_POLLS);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_read_sample(&dev, &sample));
    TEST_ASSERT_EQUAL_MEMORY((const uint8_t[sizeof(sample)]){ 0U },
                             &sample,
                             sizeof(sample));
    TEST_ASSERT_EQUAL_INT(DRDY_TIMEOUT_POLLS, gpio_calls);
    TEST_ASSERT_EQUAL_INT(1, spi_calls);
    TEST_ASSERT_EQUAL_size_t(DRDY_TIMEOUT_POLLS,
                             test_wait_wrapper_call_count());
}

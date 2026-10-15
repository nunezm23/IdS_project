#include "unity.h"

#include "ads1263.h"
#include "mock_ads1263_gpio_linux.h"
#include "mock_ads1263_spi_linux.h"

#include <string.h>

typedef struct {
    uint8_t tx[3];
    uint8_t rx[3];
    size_t len;
    ads1263_status_t result;
} transfer_step_t;

static ads1263_spi_linux_t *expected_spi;
static transfer_step_t steps[3];
static int step_count;
static int transfer_calls;

static ads1263_t make_device(void)
{
    ads1263_t dev;

    memset(&dev, 0, sizeof(dev));
    return dev;
}

static void add_transfer_step(const uint8_t *tx,
                              const uint8_t *rx,
                              size_t len,
                              ads1263_status_t result)
{
    TEST_ASSERT_LESS_THAN_INT((int)(sizeof(steps) / sizeof(steps[0])), step_count);
    steps[step_count].len = len;
    steps[step_count].result = result;
    memcpy(steps[step_count].tx, tx, len);
    memcpy(steps[step_count].rx, rx, len);
    step_count++;
}

static ads1263_status_t transfer_callback(ads1263_spi_linux_t *spi,
                                           const uint8_t *tx,
                                           uint8_t *rx,
                                           size_t len,
                                           int call_index)
{
    const transfer_step_t *step;

    TEST_ASSERT_EQUAL_PTR(expected_spi, spi);
    TEST_ASSERT_EQUAL_INT(transfer_calls, call_index);
    TEST_ASSERT_LESS_THAN_INT(step_count, call_index);
    step = &steps[call_index];
    TEST_ASSERT_EQUAL_size_t(step->len, len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(step->tx, tx, len);
    memcpy(rx, step->rx, len);
    transfer_calls++;
    return step->result;
}

static void install_script(ads1263_t *dev)
{
    expected_spi = &dev->spi;
    ads1263_spi_linux_transfer_StubWithCallback(transfer_callback);
}

static void add_wreg_step(ads1263_status_t result)
{
    const uint8_t tx[3] = { 0x46U, 0x00U, 0x0AU };
    const uint8_t rx[3] = { 0U };

    add_transfer_step(tx, rx, sizeof(tx), result);
}

static void add_rreg_step(uint8_t readback, ads1263_status_t result)
{
    const uint8_t tx[3] = { 0x26U, 0x00U, 0x00U };
    const uint8_t rx[3] = { 0x00U, 0x00U, readback };

    add_transfer_step(tx, rx, sizeof(tx), result);
}

static void add_start_step(ads1263_status_t result)
{
    const uint8_t tx[1] = { 0x08U };
    const uint8_t rx[1] = { 0U };

    add_transfer_step(tx, rx, sizeof(tx), result);
}

void setUp(void)
{
    expected_spi = NULL;
    memset(steps, 0, sizeof(steps));
    step_count = 0;
    transfer_calls = 0;
}

void tearDown(void)
{
}

void test_UT_CHAN_001_rejects_null_device_without_transfers(void)
{
    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_configure_channel(NULL, 0U));
}

void test_UT_CHAN_002_rejects_unsupported_channel_without_transfers(void)
{
    ads1263_t dev = make_device();

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_configure_channel(&dev, 1U));
}

void test_UT_CHAN_003_write_failure_suppresses_readback_and_start(void)
{
    ads1263_t dev = make_device();

    add_wreg_step(ADS1263_ERROR_SPI_TRANSFER);
    install_script(&dev);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_configure_channel(&dev, 0U));
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_CHAN_004_read_failure_suppresses_start(void)
{
    ads1263_t dev = make_device();

    add_wreg_step(ADS1263_OK);
    add_rreg_step(0x00U, ADS1263_ERROR_SPI_TRANSFER);
    install_script(&dev);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_configure_channel(&dev, 0U));
    TEST_ASSERT_EQUAL_INT(2, transfer_calls);
}

void test_UT_CHAN_005_readback_mismatch_suppresses_start(void)
{
    ads1263_t dev = make_device();

    add_wreg_step(ADS1263_OK);
    add_rreg_step(0xFFU, ADS1263_OK);
    install_script(&dev);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_REGISTER_MISMATCH,
                      ads1263_configure_channel(&dev, 0U));
    TEST_ASSERT_EQUAL_INT(2, transfer_calls);
}

void test_UT_CHAN_006_start_failure_is_propagated_after_write_and_read(void)
{
    ads1263_t dev = make_device();

    add_wreg_step(ADS1263_OK);
    add_rreg_step(0x0AU, ADS1263_OK);
    add_start_step(ADS1263_ERROR_SPI_TRANSFER);
    install_script(&dev);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_configure_channel(&dev, 0U));
    TEST_ASSERT_EQUAL_INT(3, transfer_calls);
}

void test_UT_CHAN_007_channel_zero_uses_wreg_rreg_start_order(void)
{
    ads1263_t dev = make_device();

    add_wreg_step(ADS1263_OK);
    add_rreg_step(0x0AU, ADS1263_OK);
    add_start_step(ADS1263_OK);
    install_script(&dev);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_configure_channel(&dev, 0U));
    TEST_ASSERT_EQUAL_INT(3, transfer_calls);
}

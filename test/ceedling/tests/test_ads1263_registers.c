#include "unity.h"

#include "ads1263.h"
#include "mock_ads1263_gpio_linux.h"
#include "mock_ads1263_spi_linux.h"

#include <string.h>

static ads1263_spi_linux_t *expected_spi;
static uint8_t expected_tx[34];
static uint8_t injected_rx[34];
static size_t expected_len;
static ads1263_status_t transfer_result;
static int transfer_calls;

static ads1263_t make_device(int mock)
{
    ads1263_t dev;

    memset(&dev, 0, sizeof(dev));
    dev.config.mock = mock;
    return dev;
}

static ads1263_status_t transfer_callback(ads1263_spi_linux_t *spi,
                                           const uint8_t *tx,
                                           uint8_t *rx,
                                           size_t len,
                                           int call_index)
{
    TEST_ASSERT_EQUAL_PTR(expected_spi, spi);
    TEST_ASSERT_EQUAL_INT(transfer_calls, call_index);
    TEST_ASSERT_EQUAL_size_t(expected_len, len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_tx, tx, len);
    memcpy(rx, injected_rx, len);
    transfer_calls++;
    return transfer_result;
}

static void expect_transfer(ads1263_t *dev,
                            const uint8_t *tx,
                            const uint8_t *rx,
                            size_t len,
                            ads1263_status_t result)
{
    expected_spi = &dev->spi;
    expected_len = len;
    transfer_result = result;
    memcpy(expected_tx, tx, len);
    memcpy(injected_rx, rx, len);
    ads1263_spi_linux_transfer_StubWithCallback(transfer_callback);
}

void setUp(void)
{
    memset(expected_tx, 0, sizeof(expected_tx));
    memset(injected_rx, 0, sizeof(injected_rx));
    expected_spi = NULL;
    expected_len = 0U;
    transfer_result = ADS1263_OK;
    transfer_calls = 0;
}

void tearDown(void)
{
}

void test_UT_REG_001_A_read_rejects_null_device(void)
{
    uint8_t out = 0xCCU;

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_registers(NULL, 0U, 1U, &out));
    TEST_ASSERT_EQUAL_HEX8(0xCCU, out);
}

void test_UT_REG_001_B_read_rejects_null_output(void)
{
    ads1263_t dev = make_device(0);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_registers(&dev, 0U, 1U, NULL));
}

void test_UT_REG_001_C_write_rejects_null_device(void)
{
    const uint8_t value = 0xAAU;

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_write_registers(NULL, 0U, 1U, &value));
}

void test_UT_REG_001_D_write_rejects_null_values(void)
{
    ads1263_t dev = make_device(0);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_write_registers(&dev, 0U, 1U, NULL));
}

void test_UT_REG_002_read_first_register_uses_exact_rreg_frame(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t tx[3] = { 0x20U, 0x00U, 0x00U };
    const uint8_t rx[3] = { 0x00U, 0x00U, 0xA5U };
    uint8_t out = 0U;

    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_registers(&dev, 0U, 1U, &out));
    TEST_ASSERT_EQUAL_HEX8(0xA5U, out);
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_003_read_last_register_uses_exact_rreg_frame(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t tx[3] = { 0x3FU, 0x00U, 0x00U };
    const uint8_t rx[3] = { 0x00U, 0x00U, 0x5AU };
    uint8_t out = 0U;

    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_read_registers(&dev, 31U, 1U, &out));
    TEST_ASSERT_EQUAL_HEX8(0x5AU, out);
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_004_A_read_rejects_zero_count(void)
{
    ads1263_t dev = make_device(0);
    uint8_t out = 0xCCU;

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_registers(&dev, 0U, 0U, &out));
    TEST_ASSERT_EQUAL_HEX8(0xCCU, out);
}

void test_UT_REG_004_B_read_accepts_full_register_count(void)
{
    ads1263_t dev = make_device(0);
    uint8_t tx[34] = { 0x20U, 0x1FU };
    uint8_t rx[34] = { 0U };
    uint8_t expected[32];
    uint8_t out[32];

    for (size_t i = 0U; i < sizeof(expected); ++i) {
        expected[i] = (uint8_t)i;
        rx[i + 2U] = (uint8_t)i;
    }
    memset(out, 0xCC, sizeof(out));
    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_read_registers(&dev, 0U, 32U, out));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, out, sizeof(expected));
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_004_C_read_rejects_count_above_register_file(void)
{
    ads1263_t dev = make_device(0);
    uint8_t out[33];

    memset(out, 0xCC, sizeof(out));
    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_registers(&dev, 0U, 33U, out));
    TEST_ASSERT_EACH_EQUAL_HEX8(0xCCU, out, sizeof(out));
}

void test_UT_REG_004_D_read_rejects_start_above_last_register(void)
{
    ads1263_t dev = make_device(0);
    uint8_t out = 0xCCU;

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_read_registers(&dev, 32U, 1U, &out));
    TEST_ASSERT_EQUAL_HEX8(0xCCU, out);
}

void test_UT_REG_005_read_propagates_transfer_failure_without_changing_output(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t tx[3] = { 0x20U, 0x00U, 0x00U };
    const uint8_t rx[3] = { 0U };
    uint8_t out = 0xCCU;

    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_ERROR_SPI_TRANSFER);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_read_registers(&dev, 0U, 1U, &out));
    TEST_ASSERT_EQUAL_HEX8(0xCCU, out);
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_006_A_write_first_register_uses_exact_wreg_frame(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t value = 0xAAU;
    const uint8_t tx[3] = { 0x40U, 0x00U, 0xAAU };
    const uint8_t rx[3] = { 0U };

    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_write_registers(&dev, 0U, 1U, &value));
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_006_B_write_last_register_uses_exact_wreg_frame(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t value = 0x55U;
    const uint8_t tx[3] = { 0x5FU, 0x00U, 0x55U };
    const uint8_t rx[3] = { 0U };

    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_write_registers(&dev, 31U, 1U, &value));
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_009_A_write_rejects_zero_count(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t value = 0xAAU;

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_write_registers(&dev, 0U, 0U, &value));
}

void test_UT_REG_009_B_write_accepts_full_register_count(void)
{
    ads1263_t dev = make_device(0);
    uint8_t values[32];
    uint8_t tx[34] = { 0x40U, 0x1FU };
    const uint8_t rx[34] = { 0U };

    for (size_t i = 0U; i < sizeof(values); ++i) {
        values[i] = (uint8_t)i;
        tx[i + 2U] = (uint8_t)i;
    }
    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_write_registers(&dev, 0U, 32U, values));
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_009_C_write_rejects_count_above_register_file(void)
{
    ads1263_t dev = make_device(0);
    uint8_t values[33] = { 0U };

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_write_registers(&dev, 0U, 33U, values));
}

void test_UT_REG_009_D_write_rejects_start_above_last_register(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t value = 0xAAU;

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_write_registers(&dev, 32U, 1U, &value));
}

void test_UT_REG_010_write_propagates_transfer_failure(void)
{
    ads1263_t dev = make_device(0);
    const uint8_t value = 0xAAU;
    const uint8_t tx[3] = { 0x40U, 0x00U, 0xAAU };
    const uint8_t rx[3] = { 0U };

    expect_transfer(&dev, tx, rx, sizeof(tx), ADS1263_ERROR_SPI_TRANSFER);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_write_registers(&dev, 0U, 1U, &value));
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_REG_011_A_mock_read_returns_stored_registers_without_spi(void)
{
    ads1263_t dev = make_device(1);
    uint8_t out[2] = { 0U };

    dev.mock_registers[5] = 0x6CU;
    dev.mock_registers[6] = 0xA3U;

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_read_registers(&dev, 5U, 2U, out));
    TEST_ASSERT_EQUAL_HEX8(0x6CU, out[0]);
    TEST_ASSERT_EQUAL_HEX8(0xA3U, out[1]);
}

void test_UT_REG_011_B_mock_write_updates_stored_registers_without_spi(void)
{
    ads1263_t dev = make_device(1);
    const uint8_t values[2] = { 0x12U, 0x34U };

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_write_registers(&dev, 5U, 2U, values));
    TEST_ASSERT_EQUAL_HEX8(0x12U, dev.mock_registers[5]);
    TEST_ASSERT_EQUAL_HEX8(0x34U, dev.mock_registers[6]);
}

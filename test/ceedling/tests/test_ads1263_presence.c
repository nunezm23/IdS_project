#include "unity.h"

#include "ads1263.h"
#include "mock_ads1263_gpio_linux.h"
#include "mock_ads1263_spi_linux.h"

#include <string.h>

static ads1263_spi_linux_t *expected_spi;
static uint8_t injected_registers[5];
static ads1263_status_t transfer_result;
static int transfer_calls;

static ads1263_t make_device(void)
{
    ads1263_t dev;

    memset(&dev, 0, sizeof(dev));
    return dev;
}

static ads1263_status_t presence_transfer_callback(ads1263_spi_linux_t *spi,
                                                    const uint8_t *tx,
                                                    uint8_t *rx,
                                                    size_t len,
                                                    int call_index)
{
    const uint8_t expected_tx[7] = { 0x20U, 0x04U, 0U, 0U, 0U, 0U, 0U };

    TEST_ASSERT_EQUAL_PTR(expected_spi, spi);
    TEST_ASSERT_EQUAL_INT(transfer_calls, call_index);
    TEST_ASSERT_EQUAL_size_t(sizeof(expected_tx), len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_tx, tx, sizeof(expected_tx));
    memcpy(&rx[2], injected_registers, sizeof(injected_registers));
    transfer_calls++;
    return transfer_result;
}

static void expect_presence_transfer(ads1263_t *dev,
                                     const uint8_t registers[5],
                                     ads1263_status_t result)
{
    expected_spi = &dev->spi;
    memcpy(injected_registers, registers, sizeof(injected_registers));
    transfer_result = result;
    ads1263_spi_linux_transfer_StubWithCallback(presence_transfer_callback);
}

static void assert_snapshot(const ads1263_register_snapshot_t *snapshot,
                            uint8_t id,
                            uint8_t power,
                            uint8_t interface,
                            uint8_t mode0,
                            uint8_t mode1)
{
    TEST_ASSERT_EQUAL_HEX8(id, snapshot->id);
    TEST_ASSERT_EQUAL_HEX8(power, snapshot->power);
    TEST_ASSERT_EQUAL_HEX8(interface, snapshot->interface);
    TEST_ASSERT_EQUAL_HEX8(mode0, snapshot->mode0);
    TEST_ASSERT_EQUAL_HEX8(mode1, snapshot->mode1);
}

void setUp(void)
{
    expected_spi = NULL;
    memset(injected_registers, 0, sizeof(injected_registers));
    transfer_result = ADS1263_OK;
    transfer_calls = 0;
}

void tearDown(void)
{
}

void test_UT_PRES_001_rejects_null_device_without_changing_snapshot(void)
{
    ads1263_register_snapshot_t snapshot = { 1U, 2U, 3U, 4U, 5U };

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_check_presence(NULL, &snapshot));
    assert_snapshot(&snapshot, 1U, 2U, 3U, 4U, 5U);
}

void test_UT_PRES_002_rejects_null_snapshot(void)
{
    ads1263_t dev = make_device();

    TEST_ASSERT_EQUAL(ADS1263_ERROR_INVALID_ARG,
                      ads1263_check_presence(&dev, NULL));
}

void test_UT_PRES_003_propagates_spi_failure_and_observes_zeroed_snapshot(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot = { 1U, 2U, 3U, 4U, 5U };
    const uint8_t registers[5] = { 0U };

    expect_presence_transfer(&dev, registers, ADS1263_ERROR_SPI_TRANSFER);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_SPI_TRANSFER,
                      ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0U, 0U, 0U, 0U, 0U);
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_PRES_004_classifies_all_zero_registers(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0U, 0U, 0U, 0U, 0U };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_NOT_DETECTED_ZERO,
                      ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0U, 0U, 0U, 0U, 0U);
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_PRES_005_classifies_all_ff_registers(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_NOT_DETECTED_FLOATING,
                      ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU);
    TEST_ASSERT_EQUAL_INT(1, transfer_calls);
}

void test_UT_PRES_006_detects_power_register_mismatch(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0x23U, 0x10U, 0x05U, 0x00U, 0x80U };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_REGISTER_MISMATCH,
                      ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0x23U, 0x10U, 0x05U, 0x00U, 0x80U);
}

void test_UT_PRES_007_detects_interface_register_mismatch(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0x23U, 0x11U, 0x04U, 0x00U, 0x80U };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_REGISTER_MISMATCH,
                      ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0x23U, 0x11U, 0x04U, 0x00U, 0x80U);
}

void test_UT_PRES_008_detects_mode0_register_mismatch(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0x23U, 0x11U, 0x05U, 0x01U, 0x80U };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_REGISTER_MISMATCH,
                      ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0x23U, 0x11U, 0x05U, 0x01U, 0x80U);
}

void test_UT_PRES_009_detects_mode1_register_mismatch(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0x23U, 0x11U, 0x05U, 0x00U, 0x81U };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_ERROR_REGISTER_MISMATCH,
                      ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0x23U, 0x11U, 0x05U, 0x00U, 0x81U);
}

void test_UT_PRES_010_accepts_id_variation_when_mandatory_registers_match(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0x42U, 0x11U, 0x05U, 0x00U, 0x80U };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0x42U, 0x11U, 0x05U, 0x00U, 0x80U);
}

void test_UT_PRES_011_accepts_valid_baseline_snapshot(void)
{
    ads1263_t dev = make_device();
    ads1263_register_snapshot_t snapshot;
    const uint8_t registers[5] = { 0x23U, 0x11U, 0x05U, 0x00U, 0x80U };

    expect_presence_transfer(&dev, registers, ADS1263_OK);

    TEST_ASSERT_EQUAL(ADS1263_OK, ads1263_check_presence(&dev, &snapshot));
    assert_snapshot(&snapshot, 0x23U, 0x11U, 0x05U, 0x00U, 0x80U);
}

#include "unity.h"

#include "ads1263.h"
#include "mock_ads1263_gpio_linux.h"
#include "mock_ads1263_spi_linux.h"

#include <limits.h>
#include <math.h>

#define VOLTAGE_TOLERANCE 1e-12

void setUp(void)
{
}

void tearDown(void)
{
}

void test_UT_CFG_001_default_configuration_matches_board_defaults(void)
{
    const ads1263_config_t config = ads1263_default_config();

    TEST_ASSERT_EQUAL_STRING("/dev/spidev0.0", config.spi_device);
    TEST_ASSERT_EQUAL_STRING("/dev/gpiochip6", config.gpio_chip);
    TEST_ASSERT_EQUAL_UINT(8U, config.drdy_line);
    TEST_ASSERT_EQUAL_UINT32(1000000U, config.spi_speed_hz);
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, 2.5, config.vref);
    TEST_ASSERT_EQUAL_UINT8(1U, config.gain);
    TEST_ASSERT_EQUAL_INT(1, config.use_drdy);
    TEST_ASSERT_EQUAL_INT(0, config.mock);
}

void test_UT_VOLT_001_zero_raw_converts_to_zero(void)
{
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, 0.0,
                              ads1263_raw_to_voltage(0, 2.5, 1U));
}

void test_UT_VOLT_002_A_representative_positive_raw_converts_by_formula(void)
{
    const double expected = (1073741824.0 / 2147483647.0) * 2.5;
    const double actual = ads1263_raw_to_voltage(1073741824, 2.5, 1U);

    TEST_ASSERT_GREATER_THAN_DOUBLE(0.0, actual);
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, expected, actual);
}

void test_UT_VOLT_002_B_representative_negative_raw_converts_by_formula(void)
{
    const double expected = (-1073741824.0 / 2147483647.0) * 2.5;
    const double actual = ads1263_raw_to_voltage(-1073741824, 2.5, 1U);

    TEST_ASSERT_LESS_THAN_DOUBLE(0.0, actual);
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, expected, actual);
}

void test_UT_VOLT_003_A_positive_extreme_converts_to_vref(void)
{
    const double actual = ads1263_raw_to_voltage(INT32_MAX, 2.5, 1U);

    TEST_ASSERT_TRUE(isfinite(actual));
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, 2.5, actual);
}

void test_UT_VOLT_003_B_negative_extreme_is_finite_and_uses_signed_formula(void)
{
    const double expected = ((double)INT32_MIN / 2147483647.0) * 2.5;
    const double actual = ads1263_raw_to_voltage(INT32_MIN, 2.5, 1U);

    TEST_ASSERT_TRUE(isfinite(actual));
    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, expected, actual);
}

void test_UT_VOLT_004_A_gain_two_scales_voltage_by_half(void)
{
    const double expected = (1073741824.0 / 2147483647.0) * 1.25;

    TEST_ASSERT_DOUBLE_WITHIN(
        VOLTAGE_TOLERANCE, expected,
        ads1263_raw_to_voltage(1073741824, 2.5, 2U));
}

void test_UT_VOLT_004_B_alternate_vref_scales_voltage(void)
{
    const double expected = (1073741824.0 / 2147483647.0) * 5.0;

    TEST_ASSERT_DOUBLE_WITHIN(
        VOLTAGE_TOLERANCE, expected,
        ads1263_raw_to_voltage(1073741824, 5.0, 1U));
}

void test_UT_VOLT_005_gain_zero_matches_current_gain_one_behavior(void)
{
    const double gain_one = ads1263_raw_to_voltage(1073741824, 2.5, 1U);
    const double gain_zero = ads1263_raw_to_voltage(1073741824, 2.5, 0U);

    TEST_ASSERT_DOUBLE_WITHIN(VOLTAGE_TOLERANCE, gain_one, gain_zero);
}

#include "unity.h"

#include "ads1263.h"
#include "mock_ads1263_gpio_linux.h"
#include "mock_ads1263_spi_linux.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    uint8_t before[8];
    ads1263_t dev;
    uint8_t after[8];
} guarded_device_t;

typedef struct {
    uint8_t before[8];
    uint8_t values[2];
    uint8_t after[8];
} guarded_buffer_t;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_UT_REG_007_cross_boundary_mock_read_exposes_adjacent_object_byte(void)
{
    guarded_device_t guarded_dev;
    guarded_buffer_t guarded_out;
    uint8_t adjacent_byte;
    const size_t register_offset = offsetof(ads1263_t, mock_registers);

    memset(&guarded_dev, 0, sizeof(guarded_dev));
    memset(&guarded_out, 0xA5, sizeof(guarded_out));
    guarded_dev.dev.config.mock = 1;
    guarded_dev.dev.mock_registers[31] = 0x5AU;
    guarded_dev.dev.mock_sample_counter = 0x11223344U;
    memcpy(&adjacent_byte,
           (const uint8_t *)&guarded_dev.dev + register_offset + 32U,
           sizeof(adjacent_byte));

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_read_registers(&guarded_dev.dev,
                                             31U,
                                             2U,
                                             guarded_out.values));
    TEST_ASSERT_EQUAL_HEX8(0x5AU, guarded_out.values[0]);
    TEST_ASSERT_EQUAL_HEX8(adjacent_byte, guarded_out.values[1]);
    TEST_ASSERT_EACH_EQUAL_HEX8(0xA5U,
                                guarded_out.before,
                                sizeof(guarded_out.before));
    TEST_ASSERT_EACH_EQUAL_HEX8(0xA5U,
                                guarded_out.after,
                                sizeof(guarded_out.after));
    TEST_ASSERT_EACH_EQUAL_HEX8(0U,
                                guarded_dev.before,
                                sizeof(guarded_dev.before));
    TEST_ASSERT_EACH_EQUAL_HEX8(0U,
                                guarded_dev.after,
                                sizeof(guarded_dev.after));
}

void test_UT_REG_008_cross_boundary_mock_write_changes_byte_after_register_bank(void)
{
    guarded_device_t guarded_dev;
    ads1263_t before;
    const uint8_t values[2] = { 0x12U, 0x34U };
    const size_t register_offset = offsetof(ads1263_t, mock_registers);
    const uint8_t *before_bytes;
    const uint8_t *after_bytes;

    memset(&guarded_dev, 0xA5, sizeof(guarded_dev));
    memset(&guarded_dev.dev, 0, sizeof(guarded_dev.dev));
    guarded_dev.dev.config.mock = 1;
    guarded_dev.dev.mock_registers[31] = 0x5AU;
    guarded_dev.dev.mock_sample_counter = 0x11223344U;
    before = guarded_dev.dev;

    TEST_ASSERT_EQUAL(ADS1263_OK,
                      ads1263_write_registers(&guarded_dev.dev,
                                              31U,
                                              2U,
                                              values));

    before_bytes = (const uint8_t *)&before;
    after_bytes = (const uint8_t *)&guarded_dev.dev;
    TEST_ASSERT_EQUAL_HEX8(0x12U, after_bytes[register_offset + 31U]);
    TEST_ASSERT_EQUAL_HEX8(0x34U, after_bytes[register_offset + 32U]);
    for (size_t i = 0U; i < sizeof(before); ++i) {
        if (i != register_offset + 31U && i != register_offset + 32U) {
            TEST_ASSERT_EQUAL_HEX8(before_bytes[i], after_bytes[i]);
        }
    }
    TEST_ASSERT_EACH_EQUAL_HEX8(0xA5U,
                                guarded_dev.before,
                                sizeof(guarded_dev.before));
    TEST_ASSERT_EACH_EQUAL_HEX8(0xA5U,
                                guarded_dev.after,
                                sizeof(guarded_dev.after));
}

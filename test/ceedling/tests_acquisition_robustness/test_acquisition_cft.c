#include "unity.h"

#include "acquisition.h"
#include "mock_ads1263.h"
#include "stdio_wrapper.h"
#include "wait_wrapper.h"

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_EVENTS 64U
#define MAX_EVENT_LENGTH 192U
#define MAX_SCRIPTED_READS 2U
#define CSV_HEADER "sample,raw,voltage,status\n"

typedef struct {
    unsigned int init;
    unsigned int presence;
    unsigned int configure;
    unsigned int read;
    unsigned int wait;
    unsigned int close;
    unsigned int status;
    unsigned int output;
    unsigned int open;
    unsigned int header;
    unsigned int file_close;
    unsigned int snapshot_lines;
} expected_counts_t;

static char events[MAX_EVENTS][MAX_EVENT_LENGTH];
static size_t event_count;
static const ads1263_config_t *expected_adc_config;
static ads1263_t *observed_device;

static ads1263_status_t scripted_init_status;
static ads1263_status_t scripted_presence_status;
static ads1263_status_t scripted_configure_status;
static ads1263_register_snapshot_t scripted_snapshot;
static ads1263_status_t scripted_read_status[MAX_SCRIPTED_READS];
static ads1263_sample_t scripted_samples[MAX_SCRIPTED_READS];
static size_t scripted_read_count;

static unsigned int init_calls;
static unsigned int presence_calls;
static unsigned int configure_calls;
static unsigned int read_calls;
static unsigned int close_calls;
static unsigned int status_calls;
static unsigned int sample_calls;
static unsigned int csv_sample_calls;
static ads1263_sample_t observed_samples[MAX_SCRIPTED_READS];
static ads1263_sample_t observed_csv_samples[MAX_SCRIPTED_READS];
static FILE *observed_csv_streams[MAX_SCRIPTED_READS];
static char temporary_csv_path[96];
static int temporary_csv_exists;

static void record_event(const char *format, ...)
{
    va_list args;

    TEST_ASSERT_LESS_THAN_UINT(MAX_EVENTS, event_count);
    va_start(args, format);
    (void)vsnprintf(events[event_count], sizeof(events[event_count]), format, args);
    va_end(args);
    event_count++;
}

static const char *status_name(ads1263_status_t status)
{
    switch (status) {
    case ADS1263_OK:
        return "OK";
    case ADS1263_ERROR_SPI_OPEN:
        return "SPI_OPEN";
    case ADS1263_ERROR_SPI_CONFIG:
        return "SPI_CONFIG";
    case ADS1263_ERROR_SPI_TRANSFER:
        return "SPI_TRANSFER";
    case ADS1263_ERROR_GPIO_OPEN:
        return "GPIO_OPEN";
    case ADS1263_ERROR_IO:
        return "IO";
    default:
        return "UNEXPECTED_STATUS";
    }
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
                                                     uint32_t rate_hz,
                                                     uint8_t channel,
                                                     const char *csv_path)
{
    acquisition_config_t config;

    memset(&config, 0, sizeof(config));
    config.samples = samples;
    config.rate_hz = rate_hz;
    config.channel = channel;
    config.csv_path = csv_path;
    return config;
}

static ads1263_register_snapshot_t make_snapshot(uint8_t id,
                                                 uint8_t power,
                                                 uint8_t interface,
                                                 uint8_t mode0,
                                                 uint8_t mode1)
{
    ads1263_register_snapshot_t snapshot;

    snapshot.id = id;
    snapshot.power = power;
    snapshot.interface = interface;
    snapshot.mode0 = mode0;
    snapshot.mode1 = mode1;
    return snapshot;
}

static ads1263_sample_t make_sample(int32_t raw,
                                    double voltage,
                                    uint8_t checksum)
{
    ads1263_sample_t sample;

    sample.raw = raw;
    sample.voltage = voltage;
    sample.drdy = 0;
    sample.status_byte = 0x40U;
    sample.checksum_byte = checksum;
    sample.status = "OK";
    return sample;
}

static void assert_sample_equal(const ads1263_sample_t *expected,
                                const ads1263_sample_t *actual)
{
    TEST_ASSERT_EQUAL_INT32(expected->raw, actual->raw);
    TEST_ASSERT_EQUAL_MEMORY(&expected->voltage,
                             &actual->voltage,
                             sizeof(expected->voltage));
    TEST_ASSERT_EQUAL_INT(expected->drdy, actual->drdy);
    TEST_ASSERT_EQUAL_HEX8(expected->status_byte, actual->status_byte);
    TEST_ASSERT_EQUAL_HEX8(expected->checksum_byte, actual->checksum_byte);
    TEST_ASSERT_EQUAL_STRING(expected->status, actual->status);
}

static ads1263_status_t init_callback(ads1263_t *dev,
                                      const ads1263_config_t *config,
                                      int call_index)
{
    TEST_ASSERT_EQUAL_UINT(init_calls, (unsigned int)call_index);
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_EQUAL_PTR(expected_adc_config, config);
    observed_device = dev;
    init_calls++;
    record_event("ADS:init");
    return scripted_init_status;
}

static ads1263_status_t presence_callback(
    ads1263_t *dev,
    ads1263_register_snapshot_t *snapshot,
    int call_index)
{
    TEST_ASSERT_EQUAL_UINT(presence_calls, (unsigned int)call_index);
    TEST_ASSERT_EQUAL_PTR(observed_device, dev);
    TEST_ASSERT_NOT_NULL(snapshot);
    *snapshot = scripted_snapshot;
    presence_calls++;
    record_event("ADS:presence");
    return scripted_presence_status;
}

static ads1263_status_t configure_callback(ads1263_t *dev,
                                           uint8_t channel,
                                           int call_index)
{
    TEST_ASSERT_EQUAL_UINT(configure_calls, (unsigned int)call_index);
    TEST_ASSERT_EQUAL_PTR(observed_device, dev);
    TEST_ASSERT_EQUAL_UINT8(0U, channel);
    configure_calls++;
    record_event("ADS:configure:%u", channel);
    return scripted_configure_status;
}

static ads1263_status_t read_callback(ads1263_t *dev,
                                      ads1263_sample_t *sample,
                                      int call_index)
{
    TEST_ASSERT_EQUAL_UINT(read_calls, (unsigned int)call_index);
    TEST_ASSERT_EQUAL_PTR(observed_device, dev);
    TEST_ASSERT_NOT_NULL(sample);
    TEST_ASSERT_LESS_THAN_size_t(scripted_read_count, (size_t)call_index);
    *sample = scripted_samples[call_index];
    read_calls++;
    record_event("ADS:read:%u", read_calls);
    return scripted_read_status[call_index];
}

static void close_callback(ads1263_t *dev, int call_index)
{
    TEST_ASSERT_EQUAL_UINT(close_calls, (unsigned int)call_index);
    TEST_ASSERT_EQUAL_PTR(observed_device, dev);
    close_calls++;
    record_event("ADS:close");
}

static const char *status_callback(ads1263_status_t status, int call_index)
{
    const char *name = status_name(status);

    TEST_ASSERT_EQUAL_UINT(status_calls, (unsigned int)call_index);
    TEST_ASSERT_NOT_EQUAL(0, strcmp("UNEXPECTED_STATUS", name));
    status_calls++;
    record_event("ADS:status_to_string:%s", name);
    return name;
}

static void wait_event_callback(const struct timespec *duration,
                                size_t call_index)
{
    TEST_ASSERT_NOT_NULL(duration);
    TEST_ASSERT_EQUAL_size_t(test_wait_wrapper_call_count() - 1U, call_index);
    record_event("WAIT:%ld.%09ld", duration->tv_sec, duration->tv_nsec);
}

static void stdio_event_callback(const char *operation, const char *detail)
{
    if (strcmp(operation, "fopen") == 0) {
        record_event("FILE:open:%s", detail);
    } else if (strcmp(operation, "fputs") == 0) {
        record_event("FILE:header:%s", detail);
    } else if (strcmp(operation, "fclose") == 0) {
        record_event("FILE:close");
    } else if (strcmp(operation, "printf") == 0) {
        record_event("PRINT:%s", detail);
    } else if (strncmp(operation, "unexpected_", 11U) == 0) {
        record_event("STDIO:%s:%s", operation,
                     detail == NULL ? "" : detail);
    } else {
        TEST_FAIL_MESSAGE("unknown stdio wrapper event");
    }
}

static void record_log(const char *level, const char *format, va_list args)
{
    char rendered[144];

    (void)vsnprintf(rendered, sizeof(rendered), format, args);
    record_event("%s:%s", level, rendered);
}

void app_log_info(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    record_log("INFO", format, args);
    va_end(args);
}

void app_log_warn(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    record_log("WARN", format, args);
    va_end(args);
}

void app_log_error(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    record_log("ERROR", format, args);
    va_end(args);
}

void app_log_sample(uint32_t sample_index, const ads1263_sample_t *sample)
{
    TEST_ASSERT_EQUAL_UINT32(sample_calls + 1U, sample_index);
    TEST_ASSERT_LESS_THAN_UINT(MAX_SCRIPTED_READS, sample_calls);
    TEST_ASSERT_NOT_NULL(sample);
    assert_sample_equal(&scripted_samples[sample_index - 1U], sample);
    observed_samples[sample_calls] = *sample;
    sample_calls++;
    record_event("OUT:sample:%u", sample_index);
}

void app_log_csv_sample(FILE *stream,
                        uint32_t sample_index,
                        const ads1263_sample_t *sample)
{
    TEST_ASSERT_EQUAL_UINT32(csv_sample_calls + 1U, sample_index);
    TEST_ASSERT_LESS_THAN_UINT(MAX_SCRIPTED_READS, csv_sample_calls);
    TEST_ASSERT_NOT_NULL(sample);
    assert_sample_equal(&scripted_samples[sample_index - 1U], sample);
    observed_csv_samples[csv_sample_calls] = *sample;
    observed_csv_streams[csv_sample_calls] = stream;
    csv_sample_calls++;
    record_event("OUT:csv:%u:%s", sample_index,
                 stream == NULL ? "null" : "stream");
}

static void script_read(size_t index,
                        ads1263_status_t status,
                        ads1263_sample_t sample)
{
    TEST_ASSERT_LESS_THAN_size_t(MAX_SCRIPTED_READS, index);
    scripted_read_status[index] = status;
    scripted_samples[index] = sample;
    if (scripted_read_count <= index) {
        scripted_read_count = index + 1U;
    }
}

static int run_case(const ads1263_config_t *adc,
                    const acquisition_config_t *acq,
                    int force_csv_open_failure)
{
    int result;
    const char *expected_write = NULL;

    expected_adc_config = adc;
    if (acq != NULL && acq->csv_path != NULL &&
        force_csv_open_failure == 0) {
        expected_write = CSV_HEADER;
    }
    test_stdio_wrapper_begin(acq == NULL ? NULL : acq->csv_path,
                             expected_write,
                             force_csv_open_failure,
                             stdio_event_callback);
    result = acquisition_run_samples(adc, acq);
    test_stdio_wrapper_end();
    return result;
}

static void assert_event_trace(const char *const *expected, size_t count)
{
    TEST_ASSERT_EQUAL_size_t(count, event_count);
    for (size_t i = 0U; i < count && i < event_count; ++i) {
        TEST_ASSERT_EQUAL_STRING_MESSAGE(expected[i], events[i], events[i]);
    }
}

#define ASSERT_EVENT_TRACE(...)                                                \
    do {                                                                       \
        const char *expected_events_[] = { __VA_ARGS__ };                      \
        assert_event_trace(expected_events_,                                   \
                           sizeof(expected_events_) /                          \
                               sizeof(expected_events_[0]));                   \
    } while (0)

static void assert_no_unexpected_stdio(void)
{
    size_t unexpected = test_stdio_wrapper_unexpected_count();

    if (unexpected != 0U) {
        char message[MAX_EVENT_LENGTH];
        const char *operation = test_stdio_wrapper_unexpected_operation(0U);
        const char *detail = test_stdio_wrapper_unexpected_detail(0U);

        (void)snprintf(message, sizeof(message),
                       "unexpected active stdio: %s (%s)",
                       operation == NULL ? "unknown" : operation,
                       detail == NULL ? "" : detail);
        TEST_FAIL_MESSAGE(message);
    }
}

static void assert_counts(expected_counts_t expected)
{
    assert_no_unexpected_stdio();
    TEST_ASSERT_EQUAL_UINT(expected.init, init_calls);
    TEST_ASSERT_EQUAL_UINT(expected.presence, presence_calls);
    TEST_ASSERT_EQUAL_UINT(expected.configure, configure_calls);
    TEST_ASSERT_EQUAL_UINT(expected.read, read_calls);
    TEST_ASSERT_EQUAL_size_t(expected.wait, test_wait_wrapper_call_count());
    TEST_ASSERT_EQUAL_UINT(expected.close, close_calls);
    TEST_ASSERT_EQUAL_UINT(expected.status, status_calls);
    TEST_ASSERT_EQUAL_UINT(expected.output, sample_calls);
    TEST_ASSERT_EQUAL_UINT(expected.output, csv_sample_calls);
    TEST_ASSERT_EQUAL_size_t(expected.open, test_stdio_wrapper_open_count());
    TEST_ASSERT_EQUAL_size_t(expected.header, test_stdio_wrapper_write_count());
    TEST_ASSERT_EQUAL_size_t(expected.file_close,
                             test_stdio_wrapper_close_count());
    TEST_ASSERT_EQUAL_size_t(expected.snapshot_lines,
                             test_stdio_wrapper_printf_count());
}

static void assert_csv_file_contents(const char *expected)
{
    char contents[64];
    FILE *stream = fopen(temporary_csv_path, "r");
    size_t bytes;

    TEST_ASSERT_NOT_NULL(stream);
    memset(contents, 0, sizeof(contents));
    bytes = fread(contents, 1U, sizeof(contents) - 1U, stream);
    TEST_ASSERT_EQUAL_INT(0, fclose(stream));
    TEST_ASSERT_EQUAL_size_t(strlen(expected), bytes);
    TEST_ASSERT_EQUAL_STRING(expected, contents);
}

static void assert_csv_header_effect(void)
{
    assert_csv_file_contents(CSV_HEADER);
    TEST_ASSERT_EQUAL_STRING(CSV_HEADER, test_stdio_wrapper_write(0U));
}

static int create_unique_csv_file(void)
{
    static const char path_template[] = "/tmp/ids_acquisition_cft_XXXXXX";
    int descriptor;

    TEST_ASSERT_EQUAL_INT(0, temporary_csv_exists);
    (void)snprintf(temporary_csv_path, sizeof(temporary_csv_path), "%s",
                   path_template);
    descriptor = mkstemp(temporary_csv_path);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, descriptor);
    temporary_csv_exists = 1;
    TEST_ASSERT_EQUAL_INT(0, access(temporary_csv_path, F_OK));
    return descriptor;
}

static void seed_csv_file(void)
{
    static const char stale_contents[] =
        "stale-content-that-must-be-truncated\n";
    int descriptor = create_unique_csv_file();
    ssize_t bytes_written;

    bytes_written = write(descriptor, stale_contents,
                          sizeof(stale_contents) - 1U);
    TEST_ASSERT_EQUAL_INT64((int64_t)(sizeof(stale_contents) - 1U),
                            (int64_t)bytes_written);
    TEST_ASSERT_EQUAL_INT(0, close(descriptor));
    assert_csv_file_contents(stale_contents);
}

static void make_unique_absent_csv_path(void)
{
    int descriptor = create_unique_csv_file();

    TEST_ASSERT_EQUAL_INT(0, close(descriptor));
    TEST_ASSERT_EQUAL_INT(0, remove(temporary_csv_path));
    temporary_csv_exists = 0;
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, access(temporary_csv_path, F_OK));
    TEST_ASSERT_EQUAL_INT(ENOENT, errno);
}

static void assert_csv_file_cleanup(void)
{
    TEST_ASSERT_EQUAL_INT(1, temporary_csv_exists);
    TEST_ASSERT_EQUAL_INT(0, remove(temporary_csv_path));
    temporary_csv_exists = 0;
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, access(temporary_csv_path, F_OK));
    TEST_ASSERT_EQUAL_INT(ENOENT, errno);
}

void setUp(void)
{
    event_count = 0U;
    expected_adc_config = NULL;
    observed_device = NULL;
    scripted_init_status = ADS1263_OK;
    scripted_presence_status = ADS1263_OK;
    scripted_configure_status = ADS1263_OK;
    scripted_snapshot = make_snapshot(0x01U, 0x11U, 0x05U, 0x00U, 0x80U);
    memset(scripted_read_status, 0, sizeof(scripted_read_status));
    memset(scripted_samples, 0, sizeof(scripted_samples));
    scripted_read_count = 0U;
    init_calls = 0U;
    presence_calls = 0U;
    configure_calls = 0U;
    read_calls = 0U;
    close_calls = 0U;
    status_calls = 0U;
    sample_calls = 0U;
    csv_sample_calls = 0U;
    memset(observed_samples, 0, sizeof(observed_samples));
    memset(observed_csv_samples, 0, sizeof(observed_csv_samples));
    memset(observed_csv_streams, 0, sizeof(observed_csv_streams));
    temporary_csv_path[0] = '\0';
    temporary_csv_exists = 0;

    test_wait_wrapper_reset();
    test_wait_wrapper_set_event_callback(wait_event_callback);
    test_stdio_wrapper_reset();

    ads1263_init_StubWithCallback(init_callback);
    ads1263_check_presence_StubWithCallback(presence_callback);
    ads1263_configure_channel_StubWithCallback(configure_callback);
    ads1263_read_sample_StubWithCallback(read_callback);
    ads1263_close_StubWithCallback(close_callback);
    ads1263_status_to_string_StubWithCallback(status_callback);
}

void tearDown(void)
{
    test_stdio_wrapper_end();
    if (temporary_csv_exists != 0) {
        (void)remove(temporary_csv_path);
        temporary_csv_exists = 0;
    }
}

void test_CFT_P01_null_adc_short_circuits_without_observations(void)
{
    acquisition_config_t acq = make_acquisition_config(1U, 1U, 0U, NULL);

    TEST_ASSERT_EQUAL_INT(-1, run_case(NULL, &acq, 0));
    assert_counts((expected_counts_t){ 0 });
    TEST_ASSERT_EQUAL_size_t(0U, event_count);
}

void test_CFT_P02_null_acquisition_config_logs_only_validation_error(void)
{
    ads1263_config_t adc = make_adc_config();

    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, NULL, 0));
    assert_counts((expected_counts_t){ 0 });
    ASSERT_EVENT_TRACE("ERROR:invalid acquisition configuration");
}

void test_CFT_P03_samples_zero_is_rejected_before_ads_or_file_calls(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(0U, 1U, 0U, NULL);

    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ 0 });
    ASSERT_EVENT_TRACE("ERROR:invalid samples value. Valid range: 1..1000");
}

void test_CFT_P04_samples_above_upper_bound_is_rejected(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1001U, 1U, 0U, NULL);

    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ 0 });
    ASSERT_EVENT_TRACE("ERROR:invalid samples value. Valid range: 1..1000");
}

void test_CFT_P05_rate_zero_is_rejected_before_ads_or_wait_calls(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 0U, 0U, NULL);

    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ 0 });
    ASSERT_EVENT_TRACE("ERROR:invalid rate-hz value. Valid range: 1..10");
}

void test_CFT_P06_rate_above_upper_bound_is_rejected(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 11U, 0U, NULL);

    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ 0 });
    ASSERT_EVENT_TRACE("ERROR:invalid rate-hz value. Valid range: 1..10");
}

void test_CFT_P07_unsupported_channel_is_rejected_before_configuration(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 1U, 1U, NULL);

    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ 0 });
    ASSERT_EVENT_TRACE("ERROR:invalid channel value. Valid range: 0..0");
}

void test_CFT_P08_three_invalid_fields_log_all_errors_in_order(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(0U, 11U, 1U, NULL);

    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ 0 });
    ASSERT_EVENT_TRACE(
        "ERROR:invalid samples value. Valid range: 1..1000",
        "ERROR:invalid rate-hz value. Valid range: 1..10",
        "ERROR:invalid channel value. Valid range: 0..0");
}

void test_CFT_P09_spi_open_init_failure_has_specific_guidance_and_no_close(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_init_status = ADS1263_ERROR_SPI_OPEN;
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ .init = 1U });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init",
        "ERROR:failed to open SPI device: /dev/fake-spi",
        "ERROR:If /dev/spidev0.0 is missing, bind spi0.0 to spidev first.");
}

void test_CFT_P10_gpio_open_init_failure_has_specific_guidance_and_no_close(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_init_status = ADS1263_ERROR_GPIO_OPEN;
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ .init = 1U });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init",
        "ERROR:failed to open DRDY GPIO: chip=fakechip line=8",
        "ERROR:Check gpioinfo -c gpiochip6 and verify PG8 is available.");
}

void test_CFT_P11_generic_init_failure_uses_status_text_and_no_close(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_init_status = ADS1263_ERROR_SPI_CONFIG;
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){ .init = 1U, .status = 1U });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init",
        "ADS:status_to_string:SPI_CONFIG",
        "ERROR:failed to initialize ADS1263: SPI_CONFIG");
}

void test_CFT_P12_zero_presence_prints_snapshot_logs_guidance_and_closes(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_presence_status = ADS1263_ERROR_NOT_DETECTED_ZERO;
    scripted_snapshot = make_snapshot(0U, 0U, 0U, 0U, 0U);
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .close = 1U, .snapshot_lines = 5U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init", "ADS:presence",
        "PRINT:ID        = 0x00\n", "PRINT:POWER     = 0x00\n",
        "PRINT:INTERFACE = 0x00\n", "PRINT:MODE0     = 0x00\n",
        "PRINT:MODE1     = 0x00\n",
        "ERROR:ADS1263 not detected: all registers read as 0x00",
        "ERROR:Check SPI wiring, ADS1263 power, GND, CS, MISO, MOSI, SCLK and SPI mode.",
        "ADS:close");
}

void test_CFT_P13_floating_presence_prints_snapshot_logs_guidance_and_closes(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_presence_status = ADS1263_ERROR_NOT_DETECTED_FLOATING;
    scripted_snapshot = make_snapshot(0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU);
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .close = 1U, .snapshot_lines = 5U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init", "ADS:presence",
        "PRINT:ID        = 0xFF\n", "PRINT:POWER     = 0xFF\n",
        "PRINT:INTERFACE = 0xFF\n", "PRINT:MODE0     = 0xFF\n",
        "PRINT:MODE1     = 0xFF\n",
        "ERROR:ADS1263 not detected: all registers read as 0xFF",
        "ERROR:Check floating MISO, CS wiring, ADS1263 power, GND and SPI mode.",
        "ADS:close");
}

void test_CFT_P14_mismatch_presence_logs_baseline_and_closes(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_presence_status = ADS1263_ERROR_REGISTER_MISMATCH;
    scripted_snapshot = make_snapshot(0x01U, 0x11U, 0x05U, 0x00U, 0x00U);
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .close = 1U, .snapshot_lines = 5U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init", "ADS:presence",
        "PRINT:ID        = 0x01\n", "PRINT:POWER     = 0x11\n",
        "PRINT:INTERFACE = 0x05\n", "PRINT:MODE0     = 0x00\n",
        "PRINT:MODE1     = 0x00\n",
        "ERROR:ADS1263 register baseline mismatch",
        "ERROR:Expected POWER=0x11 INTERFACE=0x05 MODE0=0x00 MODE1=0x80",
        "ADS:close");
}

void test_CFT_P15_io_presence_uses_status_text_after_snapshot_and_closes(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_presence_status = ADS1263_ERROR_IO;
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .close = 1U, .status = 1U,
        .snapshot_lines = 5U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init", "ADS:presence",
        "PRINT:ID        = 0x01\n", "PRINT:POWER     = 0x11\n",
        "PRINT:INTERFACE = 0x05\n", "PRINT:MODE0     = 0x00\n",
        "PRINT:MODE1     = 0x80\n",
        "ADS:status_to_string:IO", "ERROR:ADS1263 check failed: IO",
        "ADS:close");
}

void test_CFT_P16_configure_failure_suppresses_loop_and_closes(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 2U, 0U, NULL);

    scripted_configure_status = ADS1263_ERROR_SPI_TRANSFER;
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .close = 1U,
        .status = 1U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=2",
        "ADS:init", "ADS:presence", "ADS:configure:0",
        "ADS:status_to_string:SPI_TRANSFER",
        "ERROR:failed to configure ADS1263 channel 0: SPI_TRANSFER",
        "ADS:close");
}

void test_CFT_P17_one_sample_without_csv_succeeds_without_sleep(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 1U, 0U, NULL);
    ads1263_sample_t sample = make_sample(100, 0.125, 0xA1U);

    script_read(0U, ADS1263_OK, sample);
    TEST_ASSERT_EQUAL_INT(0, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 1U,
        .close = 1U, .output = 1U
    });
    TEST_ASSERT_NULL(observed_csv_streams[0]);
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=1",
        "ADS:init", "ADS:presence", "ADS:configure:0", "ADS:read:1",
        "OUT:sample:1", "OUT:csv:1:null", "ADS:close");
}

void test_CFT_P18_csv_open_failure_warns_and_continues_without_stream(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq;
    ads1263_sample_t sample = make_sample(100, 0.125, 0xA1U);
    char open_event[MAX_EVENT_LENGTH];
    char warning_event[MAX_EVENT_LENGTH];

    make_unique_absent_csv_path();
    acq = make_acquisition_config(1U, 1U, 0U, temporary_csv_path);
    script_read(0U, ADS1263_OK, sample);
    TEST_ASSERT_EQUAL_INT(0, run_case(&adc, &acq, 1));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 1U,
        .close = 1U, .output = 1U, .open = 1U
    });
    TEST_ASSERT_NULL(observed_csv_streams[0]);
    errno = 0;
    TEST_ASSERT_EQUAL_INT(-1, access(temporary_csv_path, F_OK));
    TEST_ASSERT_EQUAL_INT(ENOENT, errno);
    (void)snprintf(open_event, sizeof(open_event), "FILE:open:%s",
                   temporary_csv_path);
    (void)snprintf(warning_event, sizeof(warning_event),
                   "WARN:could not open CSV path: %s", temporary_csv_path);
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=1",
        "ADS:init", "ADS:presence", "ADS:configure:0",
        open_event, warning_event,
        "WARN:continuing with console output only",
        "ADS:read:1", "OUT:sample:1", "OUT:csv:1:null", "ADS:close");
}

void test_CFT_P19_csv_success_truncates_writes_header_outputs_and_closes_first(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq;
    ads1263_sample_t sample = make_sample(100, 0.125, 0xA1U);
    char open_event[MAX_EVENT_LENGTH];

    seed_csv_file();
    acq = make_acquisition_config(1U, 1U, 0U, temporary_csv_path);
    script_read(0U, ADS1263_OK, sample);
    TEST_ASSERT_EQUAL_INT(0, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 1U,
        .close = 1U, .output = 1U, .open = 1U, .header = 1U,
        .file_close = 1U
    });
    TEST_ASSERT_EQUAL_PTR(test_stdio_wrapper_stream(), observed_csv_streams[0]);
    TEST_ASSERT_NOT_NULL(observed_csv_streams[0]);
    assert_csv_header_effect();
    (void)snprintf(open_event, sizeof(open_event), "FILE:open:%s",
                   temporary_csv_path);
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=1",
        "ADS:init", "ADS:presence", "ADS:configure:0", open_event,
        "FILE:header:sample,raw,voltage,status\n", "ADS:read:1",
        "OUT:sample:1", "OUT:csv:1:stream", "FILE:close", "ADS:close");
    assert_csv_file_cleanup();
}

void test_CFT_P20_first_read_failure_has_no_output_or_sleep_and_closes(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(1U, 1U, 0U, NULL);

    script_read(0U, ADS1263_ERROR_SPI_TRANSFER,
                make_sample(0, 0.0, 0U));
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 1U,
        .close = 1U, .status = 1U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1 rate_hz=1",
        "ADS:init", "ADS:presence", "ADS:configure:0", "ADS:read:1",
        "ADS:status_to_string:SPI_TRANSFER",
        "ERROR:sample read failed: SPI_TRANSFER", "ADS:close");
}

void test_CFT_P21_two_samples_have_one_back_edge_and_one_sleep(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(2U, 2U, 0U, NULL);

    script_read(0U, ADS1263_OK, make_sample(100, 0.125, 0xA1U));
    script_read(1U, ADS1263_OK, make_sample(-100, -0.125, 0xA2U));
    TEST_ASSERT_EQUAL_INT(0, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 2U,
        .wait = 1U, .close = 1U, .output = 2U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=2 rate_hz=2",
        "ADS:init", "ADS:presence", "ADS:configure:0", "ADS:read:1",
        "OUT:sample:1", "OUT:csv:1:null", "WAIT:0.500000000",
        "ADS:read:2", "OUT:sample:2", "OUT:csv:2:null", "ADS:close");
}

void test_CFT_P22_second_read_failure_preserves_first_output_and_closes(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(2U, 2U, 0U, NULL);

    script_read(0U, ADS1263_OK, make_sample(100, 0.125, 0xA1U));
    script_read(1U, ADS1263_ERROR_SPI_TRANSFER,
                make_sample(0, 0.0, 0U));
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 2U,
        .wait = 1U, .close = 1U, .status = 1U, .output = 1U
    });
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=2 rate_hz=2",
        "ADS:init", "ADS:presence", "ADS:configure:0", "ADS:read:1",
        "OUT:sample:1", "OUT:csv:1:null", "WAIT:0.500000000",
        "ADS:read:2", "ADS:status_to_string:SPI_TRANSFER",
        "ERROR:sample read failed: SPI_TRANSFER", "ADS:close");
}

void test_CFT_P23_eintr_retries_with_scripted_remainder_before_second_read(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq = make_acquisition_config(2U, 2U, 0U, NULL);
    const struct timespec remainder = { 0, 250000000L };
    struct timespec first_wait;
    struct timespec second_wait;

    script_read(0U, ADS1263_OK, make_sample(100, 0.125, 0xA1U));
    script_read(1U, ADS1263_OK, make_sample(-100, -0.125, 0xA2U));
    test_wait_wrapper_fail_once_with_remainder(EINTR, &remainder);
    TEST_ASSERT_EQUAL_INT(0, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 2U,
        .wait = 2U, .close = 1U, .output = 2U
    });
    first_wait = test_wait_wrapper_duration(0U);
    second_wait = test_wait_wrapper_duration(1U);
    TEST_ASSERT_EQUAL_INT64(0, first_wait.tv_sec);
    TEST_ASSERT_EQUAL_INT64(500000000L, first_wait.tv_nsec);
    TEST_ASSERT_EQUAL_INT64(0, second_wait.tv_sec);
    TEST_ASSERT_EQUAL_INT64(250000000L, second_wait.tv_nsec);
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=2 rate_hz=2",
        "ADS:init", "ADS:presence", "ADS:configure:0", "ADS:read:1",
        "OUT:sample:1", "OUT:csv:1:null", "WAIT:0.500000000",
        "WAIT:0.250000000", "ADS:read:2", "OUT:sample:2",
        "OUT:csv:2:null", "ADS:close");
}

void test_CFT_P24_upper_sample_bound_csv_header_then_early_failure_cleanup(void)
{
    ads1263_config_t adc = make_adc_config();
    acquisition_config_t acq;
    char open_event[MAX_EVENT_LENGTH];

    seed_csv_file();
    acq = make_acquisition_config(1000U, 10U, 0U, temporary_csv_path);
    script_read(0U, ADS1263_ERROR_SPI_TRANSFER,
                make_sample(0, 0.0, 0U));
    TEST_ASSERT_EQUAL_INT(-1, run_case(&adc, &acq, 0));
    assert_counts((expected_counts_t){
        .init = 1U, .presence = 1U, .configure = 1U, .read = 1U,
        .close = 1U, .status = 1U, .open = 1U, .header = 1U,
        .file_close = 1U
    });
    assert_csv_header_effect();
    (void)snprintf(open_event, sizeof(open_event), "FILE:open:%s",
                   temporary_csv_path);
    ASSERT_EVENT_TRACE(
        "INFO:Acquisition configuration: channel=0 samples=1000 rate_hz=10",
        "ADS:init", "ADS:presence", "ADS:configure:0", open_event,
        "FILE:header:sample,raw,voltage,status\n", "ADS:read:1",
        "ADS:status_to_string:SPI_TRANSFER",
        "ERROR:sample read failed: SPI_TRANSFER", "FILE:close", "ADS:close");
    assert_csv_file_cleanup();
}

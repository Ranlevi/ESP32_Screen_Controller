#include "unity.h"
#include "serial_link.h"

void setUp(void)  {}
void tearDown(void) {}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static serial_link_cfg_t valid_cfg(void)
{
    return (serial_link_cfg_t)SERIAL_LINK_DEFAULT_CONFIG();
}

// ---------------------------------------------------------------------------
// Tests: serial_link_init argument validation
// These run BEFORE any successful init so that s_initialized is still false.
// ---------------------------------------------------------------------------

void test_init_null_cfg_returns_invalid_arg(void)
{
    const serial_link_stats_t *stats = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_init(NULL, &stats));
}

void test_init_null_stats_returns_invalid_arg(void)
{
    serial_link_cfg_t cfg = valid_cfg();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_init(&cfg, NULL));
}

void test_init_zero_rx_buffer_returns_invalid_arg(void)
{
    serial_link_cfg_t cfg = valid_cfg();
    cfg.rx_buffer_size = 0;
    const serial_link_stats_t *stats = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_init(&cfg, &stats));
}

void test_init_zero_tx_buffer_returns_invalid_arg(void)
{
    serial_link_cfg_t cfg = valid_cfg();
    cfg.tx_buffer_size = 0;
    const serial_link_stats_t *stats = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_init(&cfg, &stats));
}

void test_init_zero_task_stack_returns_invalid_arg(void)
{
    serial_link_cfg_t cfg = valid_cfg();
    cfg.task_stack_size = 0;
    const serial_link_stats_t *stats = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_init(&cfg, &stats));
}

void test_init_zero_task_priority_returns_invalid_arg(void)
{
    serial_link_cfg_t cfg = valid_cfg();
    cfg.task_priority = 0;
    const serial_link_stats_t *stats = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_init(&cfg, &stats));
}

// ---------------------------------------------------------------------------
// Tests: serial_link_write before init
// ---------------------------------------------------------------------------

void test_write_before_init_returns_invalid_state(void)
{
    uint8_t buf[] = { 'x' };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, serial_link_write(buf, 1));
}

// ---------------------------------------------------------------------------
// Tests: successful init and post-init behaviour
// NOTE: once init succeeds, s_initialized stays true for all subsequent tests.
// ---------------------------------------------------------------------------

void test_init_succeeds_with_valid_config(void)
{
    serial_link_cfg_t cfg = valid_cfg();
    const serial_link_stats_t *stats = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, serial_link_init(&cfg, &stats));
    TEST_ASSERT_NOT_NULL(stats);
}

void test_init_is_idempotent(void)
{
    serial_link_cfg_t cfg = valid_cfg();
    const serial_link_stats_t *stats = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, serial_link_init(&cfg, &stats));
}

void test_write_null_data_returns_invalid_arg(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_write(NULL, 1));
}

void test_write_zero_len_returns_invalid_arg(void)
{
    uint8_t buf[] = { 'x' };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, serial_link_write(buf, 0));
}

void test_write_succeeds(void)
{
    uint8_t buf[] = { 'h', 'i' };
    TEST_ASSERT_EQUAL(ESP_OK, serial_link_write(buf, sizeof(buf)));
}

// ---------------------------------------------------------------------------

int main(void)
{
    UNITY_BEGIN();

    //  Uninit tests — must run before any successful serial_link_init call.
    RUN_TEST(test_init_null_cfg_returns_invalid_arg);
    RUN_TEST(test_init_null_stats_returns_invalid_arg);
    RUN_TEST(test_init_zero_rx_buffer_returns_invalid_arg);
    RUN_TEST(test_init_zero_tx_buffer_returns_invalid_arg);
    RUN_TEST(test_init_zero_task_stack_returns_invalid_arg);
    RUN_TEST(test_init_zero_task_priority_returns_invalid_arg);
    RUN_TEST(test_write_before_init_returns_invalid_state);

    //  Post-init tests — init is called inside test_init_succeeds and stays true.
    RUN_TEST(test_init_succeeds_with_valid_config);
    RUN_TEST(test_init_is_idempotent);
    RUN_TEST(test_write_null_data_returns_invalid_arg);
    RUN_TEST(test_write_zero_len_returns_invalid_arg);
    RUN_TEST(test_write_succeeds);

    return UNITY_END();
}

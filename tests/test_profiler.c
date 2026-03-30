#include "unity.h"
#include "profiler.h"
#include "profiler_test_access.h"

#include <string.h>

void setUp(void)  {}
void tearDown(void) {}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static esp_err_t dummy_write(const uint8_t *data, size_t len)
{
    (void)data; (void)len;
    return ESP_OK;
}

static profiler_cfg_t valid_cfg(void)
{
    profiler_cfg_t cfg = PROFILER_DEFAULT_CONFIG();
    cfg.write_fn = dummy_write;
    return cfg;
}

// ---------------------------------------------------------------------------
// Tests: profiler_init argument validation
// These must run before the first successful profiler_init call.
// ---------------------------------------------------------------------------

void test_init_null_cfg_returns_invalid_arg(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, profiler_init(NULL));
}

void test_init_null_write_fn_returns_invalid_arg(void)
{
    profiler_cfg_t cfg = valid_cfg();
    cfg.write_fn = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, profiler_init(&cfg));
}

void test_init_zero_interval_returns_invalid_arg(void)
{
    profiler_cfg_t cfg = valid_cfg();
    cfg.interval_ms = 0;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, profiler_init(&cfg));
}

void test_init_zero_stack_returns_invalid_arg(void)
{
    profiler_cfg_t cfg = valid_cfg();
    cfg.task_stack_size = 0;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, profiler_init(&cfg));
}

void test_init_zero_priority_returns_invalid_arg(void)
{
    profiler_cfg_t cfg = valid_cfg();
    cfg.task_priority = 0;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, profiler_init(&cfg));
}

void test_init_succeeds_with_valid_config(void)
{
    profiler_cfg_t cfg = valid_cfg();
    TEST_ASSERT_EQUAL(ESP_OK, profiler_init(&cfg));
}

void test_init_is_idempotent(void)
{
    profiler_cfg_t cfg = valid_cfg();
    TEST_ASSERT_EQUAL(ESP_OK, profiler_init(&cfg));
}

// ---------------------------------------------------------------------------
// Tests: escape_for_json
// ---------------------------------------------------------------------------

void test_escape_plain_string_is_unchanged(void)
{
    char out[32];
    int n = escape_for_json("hello", out, sizeof(out));
    TEST_ASSERT_EQUAL(5, n);
    TEST_ASSERT_EQUAL_STRING("hello", out);
}

void test_escape_newline(void)
{
    char out[32];
    int n = escape_for_json("a\nb", out, sizeof(out));
    TEST_ASSERT_EQUAL(4, n);
    TEST_ASSERT_EQUAL_STRING("a\\nb", out);
}

void test_escape_tab(void)
{
    char out[32];
    int n = escape_for_json("a\tb", out, sizeof(out));
    TEST_ASSERT_EQUAL(4, n);
    TEST_ASSERT_EQUAL_STRING("a\\tb", out);
}

void test_escape_quote(void)
{
    char out[32];
    int n = escape_for_json("a\"b", out, sizeof(out));
    TEST_ASSERT_EQUAL(4, n);
    TEST_ASSERT_EQUAL_STRING("a\\\"b", out);
}

void test_escape_backslash(void)
{
    char out[32];
    int n = escape_for_json("a\\b", out, sizeof(out));
    TEST_ASSERT_EQUAL(4, n);
    TEST_ASSERT_EQUAL_STRING("a\\\\b", out);
}

void test_escape_carriage_return_is_dropped(void)
{
    char out[32];
    int n = escape_for_json("a\rb", out, sizeof(out));
    TEST_ASSERT_EQUAL(2, n);
    TEST_ASSERT_EQUAL_STRING("ab", out);
}

void test_escape_empty_string(void)
{
    char out[32];
    int n = escape_for_json("", out, sizeof(out));
    TEST_ASSERT_EQUAL(0, n);
    TEST_ASSERT_EQUAL_STRING("", out);
}

void test_escape_output_is_null_terminated(void)
{
    char out[8];
    escape_for_json("hi", out, sizeof(out));
    TEST_ASSERT_EQUAL('\0', out[2]);
}

void test_escape_truncates_when_buffer_full(void)
{
    //  dst_size=4 means at most 3 chars + null terminator.
    //  "ab\nc" would expand to "ab\nc" (5 chars) — must be truncated to "ab\".
    char out[4];
    escape_for_json("ab\nc", out, sizeof(out));
    TEST_ASSERT_EQUAL('\0', out[3]);   // always null-terminated
    TEST_ASSERT_TRUE(strlen(out) <= 3);
}

// ---------------------------------------------------------------------------

int main(void)
{
    UNITY_BEGIN();

    //  Uninit tests — must run before any successful profiler_init call.
    RUN_TEST(test_init_null_cfg_returns_invalid_arg);
    RUN_TEST(test_init_null_write_fn_returns_invalid_arg);
    RUN_TEST(test_init_zero_interval_returns_invalid_arg);
    RUN_TEST(test_init_zero_stack_returns_invalid_arg);
    RUN_TEST(test_init_zero_priority_returns_invalid_arg);

    //  Post-init tests.
    RUN_TEST(test_init_succeeds_with_valid_config);
    RUN_TEST(test_init_is_idempotent);

    //  escape_for_json — pure logic, no init dependency.
    RUN_TEST(test_escape_plain_string_is_unchanged);
    RUN_TEST(test_escape_newline);
    RUN_TEST(test_escape_tab);
    RUN_TEST(test_escape_quote);
    RUN_TEST(test_escape_backslash);
    RUN_TEST(test_escape_carriage_return_is_dropped);
    RUN_TEST(test_escape_empty_string);
    RUN_TEST(test_escape_output_is_null_terminated);
    RUN_TEST(test_escape_truncates_when_buffer_full);

    return UNITY_END();
}

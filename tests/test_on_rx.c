#include "unity.h"
#include "oled_stub.h"
#include "profiler_stub.h"

#include <stdint.h>
#include <string.h>

/*
    Test Infrastructure notes:
    ESP-IDF has a built-in test framework, but it is meant to run on the the
    target device only: we want to test the logic on the PC Host.
    There's also a Linux target which runs the FW as a native Linux Binary,
    but that's an overkill for our simple app.
    
    Solution: we'll create a standalone Unity framework, with simple stubs 
    to simulate ESP-IDF functions the code calls. Reason: Unity is simple
    (2 c files), and stubs are minimal. The apps code compiles unchanged
    against the stubs.

    To run the tests: cd to /tests
    cd tests
    cmake -B build -G "MinGW Makefiles"
    cmake --build build

    # Run individually:
    ./build/test_on_rx.exe
    ./build/test_serial_link.exe
    ./build/test_oled.exe

    # Or all at once:
    cmake --build build --target run_all_tests    
*/

//  Functions under test (defined in main/main.c)
void on_rx(const uint8_t *data, size_t len);
void on_rx_reset(void);

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void send(const char *s)
{
    on_rx((const uint8_t *)s, strlen(s));
}

void setUp(void)
{
    stub_reset();
    stub_profiler_reset();
    on_rx_reset();
}

void tearDown(void) {}

// ---------------------------------------------------------------------------
// Test cases
// ---------------------------------------------------------------------------

void test_displays_text_on_newline(void)
{
    send("hello\n");
    TEST_ASSERT_EQUAL_STRING("hello", stub_last_text());
}

void test_strips_carriage_return(void)
{
    send("hi\r\n");
    TEST_ASSERT_EQUAL_STRING("hi", stub_last_text());
}

void test_no_display_without_newline(void)
{
    send("hello");
    TEST_ASSERT_EQUAL_STRING("", stub_last_text());
}

void test_clear_called_before_display(void)
{
    send("hi\n");
    TEST_ASSERT_EQUAL_INT(1, stub_clear_count());
}

void test_flushes_when_line_full(void)
{
    //  16 chars exactly — should flush without a newline
    send("1234567890123456");
    TEST_ASSERT_EQUAL_STRING("1234567890123456", stub_last_text());
}

void test_overflow_char_starts_next_line(void)
{
    //  17th char overflows the full line; it should be buffered for next flush
    send("1234567890123456X\n");
    TEST_ASSERT_EQUAL_STRING("X", stub_last_text());
}

void test_empty_line(void)
{
    //  A bare newline should display an empty string, not crash
    send("\n");
    TEST_ASSERT_EQUAL_STRING("", stub_last_text());
}

void test_multiple_lines(void)
{
    send("first\n");
    send("second\n");
    TEST_ASSERT_EQUAL_STRING("second", stub_last_text());
}

void test_oled_command_does_not_update_display(void)
{
    //  OLED: prefix is a command — should not call oled_show_text
    send("OLED:free_heap_b\n");
    TEST_ASSERT_EQUAL_STRING("", stub_last_text());
    TEST_ASSERT_EQUAL_INT(0, stub_clear_count());
}

void test_oled_command_followed_by_normal_line(void)
{
    //  After an OLED: command a normal line should still display as usual
    send("OLED:uptime_s\n");
    send("hello\n");
    TEST_ASSERT_EQUAL_STRING("hello", stub_last_text());
}

void test_lowercase_oled_prefix_is_not_a_command(void)
{
    //  Matching is case-sensitive: "oled:" is not "OLED:", so the line is NOT
    //  intercepted as a command — profiler_set_oled_stat must not be called.
    //  (The text is auto-flushed to OLED at DISPLAY_COLS, then the trailing
    //  newline produces a second display call with an empty buffer — tested
    //  separately. The critical invariant here is the key stays unset.)
    send("oled:free_heap_b\n");
    TEST_ASSERT_EQUAL_STRING("", stub_last_oled_key());
    TEST_ASSERT_TRUE(stub_clear_count() >= 1);  // something was displayed, not suppressed
}

void test_oled_command_empty_key_clears_selection(void)
{
    //  "OLED:\n" passes an empty string to profiler_set_oled_stat, which
    //  clears the current selection. It must not display anything.
    send("OLED:uptime_s\n");   // set a key first
    stub_reset();               // clear display counters only
    send("OLED:\n");
    TEST_ASSERT_EQUAL_STRING("", stub_last_oled_key());
    TEST_ASSERT_EQUAL_STRING("", stub_last_text());
    TEST_ASSERT_EQUAL_INT(0, stub_clear_count());
}

// ---------------------------------------------------------------------------

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_displays_text_on_newline);
    RUN_TEST(test_strips_carriage_return);
    RUN_TEST(test_no_display_without_newline);
    RUN_TEST(test_clear_called_before_display);
    RUN_TEST(test_flushes_when_line_full);
    RUN_TEST(test_overflow_char_starts_next_line);
    RUN_TEST(test_empty_line);
    RUN_TEST(test_multiple_lines);
    RUN_TEST(test_oled_command_does_not_update_display);
    RUN_TEST(test_oled_command_followed_by_normal_line);
    RUN_TEST(test_lowercase_oled_prefix_is_not_a_command);
    RUN_TEST(test_oled_command_empty_key_clears_selection);
    return UNITY_END();
}

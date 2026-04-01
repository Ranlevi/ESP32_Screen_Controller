#include "unity.h"
#include "oled.h"
#include "esp_lcd_stub.h"

#include <string.h>
#include <stdbool.h>

void setUp(void)  { stub_lcd_reset(); }
void tearDown(void) {}

// ---------------------------------------------------------------------------
// Tests: before init
// These must run before the first oled_init() call.
// ---------------------------------------------------------------------------

void test_clear_before_init_returns_invalid_state(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, oled_clear());
}

void test_show_text_before_init_returns_invalid_state(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, oled_show_text(0, 0, "hi"));
}

// ---------------------------------------------------------------------------
// Tests: init
// ---------------------------------------------------------------------------

void test_init_succeeds(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, oled_init());
}

void test_init_is_idempotent(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, oled_init());
}

// ---------------------------------------------------------------------------
// Tests: argument validation (requires init to have succeeded)
// ---------------------------------------------------------------------------

void test_show_text_null_returns_invalid_arg(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, oled_show_text(0, 0, NULL));
}

void test_show_text_negative_col_returns_invalid_arg(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, oled_show_text(-1, 0, "x"));
}

void test_show_text_negative_row_returns_invalid_arg(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, oled_show_text(0, -1, "x"));
}

// ---------------------------------------------------------------------------
// Tests: framebuffer content
// ---------------------------------------------------------------------------

void test_clear_sends_zero_framebuffer(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, oled_clear());
    TEST_ASSERT_TRUE(stub_lcd_draw_called());

    const uint8_t *bmp = stub_lcd_bitmap();
    for (int i = 0; i < LCD_FRAMEBUFFER_SIZE; i++) {
        if (bmp[i] != 0) {
            TEST_FAIL_MESSAGE("framebuffer not zero after oled_clear");
        }
    }
}

void test_space_char_renders_blank(void)
{
    oled_clear();
    stub_lcd_reset();

    TEST_ASSERT_EQUAL(ESP_OK, oled_show_text(0, 0, " "));

    //  First 8 bytes are the 8 column-bytes for the space character at x=0.
    const uint8_t *bmp = stub_lcd_bitmap();
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_HEX8(0x00, bmp[i]);
    }
}

void test_nonspace_char_has_nonzero_pixels(void)
{
    oled_clear();
    stub_lcd_reset();

    //  'A' is not blank — at least one column byte must be non-zero.
    TEST_ASSERT_EQUAL(ESP_OK, oled_show_text(0, 0, "A"));

    const uint8_t *bmp = stub_lcd_bitmap();
    bool found_nonzero = false;
    for (int i = 0; i < 8; i++) {
        if (bmp[i] != 0) { found_nonzero = true; break; }
    }
    TEST_ASSERT_TRUE_MESSAGE(found_nonzero, "'A' should produce non-zero pixels");
}

void test_show_text_at_right_edge_does_not_crash(void)
{
    //  col=128 means the first char already clips — no bytes should be written,
    //  but the function must return ESP_OK without any out-of-bounds access.
    TEST_ASSERT_EQUAL(ESP_OK, oled_show_text(128, 0, "A"));
}

void test_blit_text_no_ghost_pixels_after_shorter_overwrite(void)
{
    //  Write a wide string to fill most of row 0, then overwrite with a
    //  single character. Any column bytes beyond the 'A' glyph (columns 8–127)
    //  must be zero — no pixels ghosting from the first write.
    oled_clear();
    oled_blit_text(0, 0, "AAAAAAAAAAAAAAAA"); // 16 chars = 128 px, fills row
    oled_blit_text(0, 0, "A");               // 1 char = 8 px, rest should clear
    oled_flush();

    const uint8_t *bmp = stub_lcd_bitmap();
    for (int col = 8; col < 128; col++) {
        if (bmp[col] != 0) {
            TEST_FAIL_MESSAGE("ghost pixels found after shorter overwrite");
        }
    }
}

// ---------------------------------------------------------------------------

int main(void)
{
    UNITY_BEGIN();

    //  Uninit tests — must run before the first oled_init().
    RUN_TEST(test_clear_before_init_returns_invalid_state);
    RUN_TEST(test_show_text_before_init_returns_invalid_state);

    //  Init tests.
    RUN_TEST(test_init_succeeds);
    RUN_TEST(test_init_is_idempotent);

    //  Post-init tests.
    RUN_TEST(test_show_text_null_returns_invalid_arg);
    RUN_TEST(test_show_text_negative_col_returns_invalid_arg);
    RUN_TEST(test_show_text_negative_row_returns_invalid_arg);
    RUN_TEST(test_clear_sends_zero_framebuffer);
    RUN_TEST(test_space_char_renders_blank);
    RUN_TEST(test_nonspace_char_has_nonzero_pixels);
    RUN_TEST(test_show_text_at_right_edge_does_not_crash);
    RUN_TEST(test_blit_text_no_ghost_pixels_after_shorter_overwrite);

    return UNITY_END();
}

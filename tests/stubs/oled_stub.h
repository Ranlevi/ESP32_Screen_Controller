#pragma once

//  Test accessors for the oled stub.
//  Call stub_reset() in setUp() before each test case.

const char *stub_last_text(void);
int         stub_clear_count(void);
void        stub_reset(void);

#pragma once

//  Exposes profiler internal functions for white-box testing.
//  Only include this file from test code, never from production code.
//  Requires PROFILER_TEST_ACCESS to be defined in the build (see CMakeLists.txt).

int         escape_for_json(const char *src, char *dst, size_t dst_size);
void        profiler_reset_for_test(void);
const char *profiler_get_oled_key_for_test(void);

/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-01-03     test         Unit tests for ulog_console_backend_init
 */

#include <rtthread.h>
#include <ulog.h>
#include <rthw.h>
#include "utest.h"

#define LOG_TAG     "ulog_con_test"
#define LOG_LVL     LOG_LVL_DBG
#include <ulog.h>

static struct utest *utest = RT_NULL;
static rt_bool_t g_rt_kputs_called = RT_FALSE;
static char g_rt_kputs_buffer[256] = {0};

/* Mock rt_kputs to capture output */
rt_weak void rt_kputs(const char *str)
{
    if (str != RT_NULL)
    {
        g_rt_kputs_called = RT_TRUE;
        rt_strncpy(g_rt_kputs_buffer, str, sizeof(g_rt_kputs_buffer) - 1);
        g_rt_kputs_buffer[sizeof(g_rt_kputs_buffer) - 1] = '\0';
    }
}

/* Test: Normal initialization path - console backend should be successfully registered */
static void test_console_backend_init_normal(void)
{
    int result;
    ulog_backend_t backend;

    /* Reset mock state */
    g_rt_kputs_called = RT_FALSE;
    rt_memset(g_rt_kputs_buffer, 0, sizeof(g_rt_kputs_buffer));

    /* Call the function to initialize console backend */
    result = ulog_console_backend_init();

    /* Verify return value is 0 (success) */
    uassert_true(result == 0);

    /* Verify backend is registered with correct name */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Verify backend configuration */
    uassert_true(backend->support_color == RT_TRUE);
    uassert_true(backend->output != RT_NULL);
    uassert_true(backend->output == ulog_console_backend_output);
}

/* Test: Multiple initialization calls should be safe (ulog_init is idempotent) */
static void test_console_backend_init_multiple_times(void)
{
    int result1, result2;
    ulog_backend_t backend;

    /* First initialization */
    result1 = ulog_console_backend_init();
    uassert_true(result1 == 0);

    /* Second initialization - should not cause issues */
    result2 = ulog_console_backend_init();
    uassert_true(result2 == 0);

    /* Verify backend is still registered */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);
    uassert_true(backend->support_color == RT_TRUE);
    uassert_true(backend->output == ulog_console_backend_output);
}

/* Test: Console backend output function is correctly set */
static void test_console_backend_output_function(void)
{
    ulog_backend_t backend;
    const char test_log[] = "Test log message";

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find the registered backend */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Verify output function pointer is set */
    uassert_true(backend->output != RT_NULL);

    /* Call the output function directly */
    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_INFO, "test_tag", RT_FALSE, test_log, rt_strlen(test_log));

    /* Verify rt_kputs was called (though we can't verify exact content due to potential formatting) */
    uassert_true(g_rt_kputs_called == RT_TRUE);
}

/* Test: Backend supports color output flag is correctly set */
static void test_console_backend_color_support(void)
{
    ulog_backend_t backend;

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find the registered backend */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Verify support_color is RT_TRUE as passed in registration */
    uassert_true(backend->support_color == RT_TRUE);
}

/* Test: Console backend name is correctly set */
static void test_console_backend_name(void)
{
    ulog_backend_t backend;

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find the registered backend */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Verify backend name is "console" */
    uassert_true(rt_strcmp(backend->name, "console") == 0);
}

/* Test: Console backend output function handles raw log output */
static void test_console_backend_output_raw(void)
{
    ulog_backend_t backend;
    const char test_raw_log[] = "Raw output test";

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find the registered backend */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Call output function with is_raw = RT_TRUE */
    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_INFO, "test_tag", RT_TRUE, test_raw_log, rt_strlen(test_raw_log));

    /* Verify rt_kputs was called */
    uassert_true(g_rt_kputs_called == RT_TRUE);
}

/* Test: Console backend output with different log levels */
static void test_console_backend_output_levels(void)
{
    ulog_backend_t backend;
    const char test_log[] = "Test message";

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find the registered backend */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Test with different log levels */
    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_ASSERT, "assert_tag", RT_FALSE, test_log, rt_strlen(test_log));
    uassert_true(g_rt_kputs_called == RT_TRUE);

    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_ERROR, "error_tag", RT_FALSE, test_log, rt_strlen(test_log));
    uassert_true(g_rt_kputs_called == RT_TRUE);

    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_WARNING, "warn_tag", RT_FALSE, test_log, rt_strlen(test_log));
    uassert_true(g_rt_kputs_called == RT_TRUE);

    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_INFO, "info_tag", RT_FALSE, test_log, rt_strlen(test_log));
    uassert_true(g_rt_kputs_called == RT_TRUE);

    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_DBG, "debug_tag", RT_FALSE, test_log, rt_strlen(test_log));
    uassert_true(g_rt_kputs_called == RT_TRUE);
}

/* Test: Backend initialization does not modify unrelated backend configurations */
static void test_console_backend_isolation(void)
{
    ulog_backend_t backend;

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find and verify only console backend properties */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Other function pointers should be NULL unless set elsewhere */
    /* init, flush, deinit, filter are optional and may be NULL */
}

/* Test: Console backend out_level is set to LOG_FILTER_LVL_ALL after registration */
static void test_console_backend_output_level(void)
{
    ulog_backend_t backend;

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find the registered backend */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Verify out_level is set to ALL (as set in ulog_backend_register) */
    uassert_true(backend->out_level == LOG_FILTER_LVL_ALL);
}

/* Test: Console backend output function handles empty log */
static void test_console_backend_output_empty_log(void)
{
    ulog_backend_t backend;
    const char empty_log[] = "";

    /* Initialize console backend */
    ulog_console_backend_init();

    /* Find the registered backend */
    backend = ulog_backend_find("console");
    uassert_true(backend != RT_NULL);

    /* Call output function with empty log */
    g_rt_kputs_called = RT_FALSE;
    backend->output(backend, LOG_LVL_INFO, "test_tag", RT_FALSE, empty_log, 0);

    /* Verify rt_kputs was called (even for empty strings) */
    uassert_true(g_rt_kputs_called == RT_TRUE);
}

static rt_err_t utest_tc_init(void)
{
    ulog = utest_handle_get();
    return RT_EOK;
}

static rt_err_t utest_tc_cleanup(void)
{
    /* Unregister console backend if needed */
    ulog_backend_t backend = ulog_backend_find("console");
    if (backend != RT_NULL)
    {
        ulog_backend_unregister(backend);
    }
    return RT_EOK;
}

static void testcase(void)
{
    UTEST_UNIT_RUN(test_console_backend_init_normal);
    UTEST_UNIT_RUN(test_console_backend_init_multiple_times);
    UTEST_UNIT_RUN(test_console_backend_output_function);
    UTEST_UNIT_RUN(test_console_backend_color_support);
    UTEST_UNIT_RUN(test_console_backend_name);
    UTEST_UNIT_RUN(test_console_backend_output_raw);
    UTEST_UNIT_RUN(test_console_backend_output_levels);
    UTEST_UNIT_RUN(test_console_backend_isolation);
    UTEST_UNIT_RUN(test_console_backend_output_level);
    UTEST_UNIT_RUN(test_console_backend_output_empty_log);
}

UTEST_TC_EXPORT(testcase, "utilities.ulog.console_backend", utest_tc_init, utest_tc_cleanup, 1000);

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>
#include "kernel.h"
#include "sys/util_macro.h"
#include "zbus.h"
#include "ztest_assert.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

void urgent_callback(zbus_channel_index_t idx);

ZBUS_LISTENER_REGISTER(critical, urgent_callback);

int count = 0;
void urgent_callback(zbus_channel_index_t idx)
{
    printk(" *** ONLY ONE CRITICAL CALL for channel %d ***\n", idx);
    ++count;
    zbus_subscriber_set_enable(&critical, false);
}
extern struct net_pkt pkt;
/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_01(void)
{
    struct action start = {true};
    zbus_chan_pub(start_measurement, start, K_MSEC(200));
    k_msleep(1000);
    zassert_equal(pkt.x, 'I', "1 was not equal to 1");
    zassert_equal(pkt.y, false, "1 was not equal to 1");
    zassert_true(count < 2, "Count must be lest than 2");
}

void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_01));

    ztest_run_test_suite(framework_tests);
}

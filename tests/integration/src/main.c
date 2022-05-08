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
    zb_chan_pub(start_measurement, start);
    k_msleep(1000);
    zassert_equal(pkt.x, 'I', "1 was not equal to 1");
    zassert_equal(pkt.y, false, "1 was not equal to 1");
}

void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_01));

    ztest_run_test_suite(framework_tests);
}

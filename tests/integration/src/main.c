/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>
#include "kernel.h"
#include "sys/util_macro.h"
#include "zbus.h"
#include "zbus_messages.h"
#include "ztest_assert.h"
#include "ztest_test_deprecated.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

int count_callback = 0;

void urgent_callback(zbus_channel_index_t idx)
{
    printk(" *** LISTENER activated for channel %d ***\n", idx);
    ++count_callback;
}
ZBUS_LISTENER_DECLARE(critical, urgent_callback);


int count_core = 0;
ZBUS_SUBSCRIBER_DECLARE(core, 1);
void core_thread(void)
{
    zbus_channel_index_t idx = 0;
    while (1) {
        if (!k_msgq_get(core.queue, &idx, K_FOREVER)) {
            count_core++;
            struct sensor_data data = {0};
            ZBUS_CHAN_READ(sensor_data, data, K_NO_WAIT);
            struct net_pkt pkt = {.total = data.a + data.b};
            LOG_DBG("[Core] sensor {a = %d, b = %d}. Sending pkt {total=%d}", data.a,
                    data.b, pkt.total);
            ZBUS_CHAN_PUB(net_pkt, pkt, K_MSEC(200));
        }
        /* simulate a slow consumer */
        k_busy_wait(100000);
    }
}
K_THREAD_DEFINE(core_thread_id, 1024, core_thread, NULL, NULL, NULL, 3, 0, 0);


struct net_pkt pkt = {0};
int count_net      = 0;
ZBUS_SUBSCRIBER_DECLARE(net, 4);
void net_thread(void)
{
    zbus_channel_index_t idx = 0;
    while (1) {
        if (!k_msgq_get(net.queue, &idx, K_FOREVER)) {
            count_net++;
            ZBUS_CHAN_READ(net_pkt, pkt, K_NO_WAIT);
            LOG_DBG("[Net] Total %d", pkt.total);
        }
    }
}
K_THREAD_DEFINE(net_thread_id, 1024, net_thread, NULL, NULL, NULL, 3, 0, 0);


int count_peripheral = 0;
int a                = 0;
int b                = 0;
ZBUS_SUBSCRIBER_DECLARE(peripheral, 16);
void peripheral_thread(void)
{
    struct sensor_data sd = {0, 0};


    zbus_channel_index_t idx = 0;
    while (!k_msgq_get(peripheral.queue, &idx, K_FOREVER)) {
        count_peripheral++;
        LOG_DBG("[Peripheral] starting measurement");
        ++a;
        sd.a = a;
        ++b;
        sd.b = b;
        LOG_DBG("[Peripheral] sending sensor data");
        ZBUS_CHAN_PUB(sensor_data, sd, K_MSEC(250));
    }
}
K_THREAD_DEFINE(peripheral_thread_id, 1024, peripheral_thread, NULL, NULL, NULL, 3, 0, 0);

static void context_reset(void)
{
    k_msleep(200);
    a                = 0;
    b                = 0;
    count_callback   = 0;
    count_core       = 0;
    count_net        = 0;
    count_peripheral = 0;
    pkt.total        = 0;
    struct net_pkt *p;
    zbus_chan_claim(ZBUS_CHAN_GET(net_pkt), (void **) &p, K_NO_WAIT);
    p->total = 0;
    zbus_chan_finish(ZBUS_CHAN_GET(net_pkt), K_NO_WAIT);
    struct sensor_data *sd;
    zbus_chan_claim(ZBUS_CHAN_GET(sensor_data), (void **) &sd, K_NO_WAIT);
    sd->a = 0;
    sd->b = 1;
    zbus_chan_finish(ZBUS_CHAN_GET(sensor_data), K_NO_WAIT);
    zbus_observer_set_enable(&critical, true);
    zbus_observer_set_enable(&peripheral, true);
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_basic(void)
{
    struct action start = {true};

    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(200);

    zassert_true(count_callback == 1, "");
    zassert_true(count_core == 1, "");
    zassert_true(count_net == 1, "");
    zassert_true(count_peripheral == 1, "");

    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(200);

    zassert_true(count_callback == 2, "");
    zassert_true(count_core == 2, "");
    zassert_true(count_net == 2, "");
    zassert_true(count_peripheral == 2, "");

    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(200);

    zassert_true(count_callback == 3, "");
    zassert_true(count_core == 3, "");
    zassert_true(count_net == 3, "");
    zassert_true(count_peripheral == 3, "");

    zassert_equal(pkt.total, 6, "result was %d", pkt.total);
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_channel_set_enable(void)
{
    struct action start = {true};

    zbus_observer_set_enable(&critical, false);
    zbus_observer_set_enable(&peripheral, false);
    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(200);

    zassert_true(count_callback == 0, "");
    zassert_true(count_core == 0, "core was %d", core);
    zassert_true(count_net == 0, "");
    zassert_true(count_peripheral == 0, "");


    zbus_observer_set_enable(&critical, false);
    zbus_observer_set_enable(&peripheral, true);
    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(200);

    zassert_true(count_callback == 0, "");
    zassert_true(count_core == 1, "");
    zassert_true(count_net == 1, "");
    zassert_true(count_peripheral == 1, "");

    zbus_observer_set_enable(&critical, true);
    zbus_observer_set_enable(&peripheral, false);
    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(200);

    zassert_true(count_callback == 1, "");
    zassert_true(count_core == 1, "");
    zassert_true(count_net == 1, "");
    zassert_true(count_peripheral == 1, "");

    zbus_observer_set_enable(&critical, true);
    zbus_observer_set_enable(&peripheral, true);
    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(200);

    zassert_true(count_callback == 2, "");
    zassert_true(count_core == 2, "");
    zassert_true(count_net == 2, "");
    zassert_true(count_peripheral == 2, "");

    zassert_equal(pkt.total, 4, "result was %d", pkt.total);
}

static void test_event_dispatcher_timeout(void)
{
    struct action start = {true};

    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(60);

    zassert_true(count_callback == 1, "");
    zassert_true(count_core == 1, "");
    zassert_true(count_net == 0, "count_net was %d", count_net);
    zassert_true(count_peripheral == 1, "");


    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(60);

    zassert_true(count_callback == 2, "");
    zassert_true(count_core == 2, "");
    zassert_true(count_net == 1, "");
    zassert_true(count_peripheral == 2, "");

    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(60);

    zassert_true(count_callback == 3, "");
    zassert_true(count_core == 3, "");
    zassert_true(count_net == 2, "");
    zassert_true(count_peripheral == 3, "");

    ZBUS_CHAN_PUB(start_measurement, start, K_MSEC(200));
    k_msleep(60);

    zassert_true(count_callback == 4, "");
    zassert_true(count_core == 3, "");
    zassert_true(count_net == 2, "");
    zassert_true(count_peripheral == 3, "");

    zassert_equal(pkt.total, 4, "result was %d", pkt.total);
}
void test_main(void)
{
    ztest_test_suite(
        framework_tests,
        ztest_unit_test_setup_teardown(test_basic, context_reset, unit_test_noop),
        ztest_unit_test_setup_teardown(test_channel_set_enable, context_reset,
                                       unit_test_noop),
        ztest_unit_test_setup_teardown(test_event_dispatcher_timeout, context_reset,
                                       unit_test_noop));

    ztest_run_test_suite(framework_tests);
}

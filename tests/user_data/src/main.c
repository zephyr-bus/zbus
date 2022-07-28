/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <ztest.h>
#include "zbus.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);
/*
 * @brief Systematic analyses of inputs
 name = str[0,1,>1]
 read_only, on_change = bool [false, true]
 message_type = [struct, union]
 observers = null ended array [0,1,>1]
 initial_value = [0, some, all]
 */


static void test_channel_user_data(void)
{
    struct zbus_channel *version_channel = ZBUS_CHAN_GET(version);
    zassert_equal(version_channel->user_data[0], 0, NULL);
    zassert_equal(ARRAY_SIZE(version_channel->user_data),
                  CONFIG_ZBUS_CHANNEL_USER_DATA_SIZE, NULL);
    memset(version_channel->user_data, 1, CONFIG_ZBUS_CHANNEL_USER_DATA_SIZE);
    for (int i = 0; i < CONFIG_ZBUS_CHANNEL_USER_DATA_SIZE; ++i) {
        zassert_equal(version_channel->user_data[i], 1, NULL);
    }
    struct zbus_channel *regular_channel = ZBUS_CHAN_GET(regular_chan);
    zassert_equal(regular_channel->user_data[0], 0, NULL);
    zassert_equal(ARRAY_SIZE(regular_channel->user_data),
                  CONFIG_ZBUS_CHANNEL_USER_DATA_SIZE, NULL);
    memset(regular_channel->user_data, 2, CONFIG_ZBUS_CHANNEL_USER_DATA_SIZE);
    for (int i = 0; i < CONFIG_ZBUS_CHANNEL_USER_DATA_SIZE; ++i) {
        zassert_equal(regular_channel->user_data[i], 2, NULL);
    }
}

int count = 0;
void urgent_callback(zbus_chan_idx_t idx)
{
    if (idx == regular_chan_index) {
        ++count;
    }
}
ZBUS_LISTENER_DECLARE(foo_listener, urgent_callback);

ZBUS_SUBSCRIBER_DECLARE(foo_subscriber, 1);
void foo_subscriber_thread(void)
{
    zbus_chan_idx_t idx = 0;
    while (1) {
        if (!k_msgq_get(foo_subscriber.queue, &idx, K_FOREVER)) {
            if (idx == regular_chan_index) {
                ++count;
            }
        }
    }
}
K_THREAD_DEFINE(foo_subscriber_thread_id, 1024, foo_subscriber_thread, NULL, NULL, NULL, 3, 0, 0);

static void test_user_data_regression(void)
{
    /* To ensure the pub/sub behavior is kept */
    struct foo_msg sent = {.a = 10, .b = 1000};
    ZBUS_CHAN_PUB(regular_chan, sent, K_NO_WAIT);
    struct foo_msg received = {0};
    ZBUS_CHAN_READ(regular_chan, received, K_NO_WAIT);
    zassert_equal(sent.a, received.a, NULL);
    zassert_equal(sent.b, received.b, NULL);

    k_msleep(1000);

    zassert_equal(count, 2, NULL);
}
void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_channel_user_data),
                     ztest_unit_test(test_user_data_regression));

    ztest_run_test_suite(framework_tests);
}

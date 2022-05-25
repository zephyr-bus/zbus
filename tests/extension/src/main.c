/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <ztest.h>
#include "kernel.h"
#include "sys/util_macro.h"
#include "zbus.h"
#include "ztest_assert.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(core, CONFIG_ZBUS_LOG_LEVEL);

extern struct k_msgq __zbus_ext_msgq;

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_01(void)
{
    zbus_channel_index_t idx        = 0;
    zbus_channel_variant_t msg_data = {0};
    uint8_t count                 = 0;
    while (1) {
        if (!k_msgq_get(&__zbus_ext_msgq, &idx, K_FOREVER)) {
            struct metadata *meta = __zbus_metadata_get_by_id(idx);
            __zbus_chan_read(meta, (uint8_t *) &msg_data, meta->message_size, K_MSEC(500));
            switch (idx) {
            case zbus_index_sensor_data: {
                ++count;
                LOG_DBG("[Extension] receiving sensor data with a=%d, b=%d",
                        msg_data.sensor_data.a, msg_data.sensor_data.b);
                zassert_true(msg_data.sensor_data.a == count, "it must be %d", count);
                zassert_true(msg_data.sensor_data.b == (count * 10),
                             "it must be %d and not %d", count * 10,
                             msg_data.sensor_data.b);
                if (count == 4) {
                    return;
                }
            } break;
            case zbus_index_start_measurement: {
                LOG_DBG("[Extension] receiving start measurement with status %d",
                        msg_data.start_measurement.status);
                zassert_true(msg_data.start_measurement.status == (count % 2),
                             "it must be %d, and not %d", (count % 2),
                             msg_data.start_measurement.status);
            } break;
            case zbus_index_finish: {
                LOG_DBG("[Extension] receiving finish with status %d",
                        msg_data.finish.status);
                zassert_true(msg_data.finish.status == true, "it must be %d, and not %d",
                             true, msg_data.finish.status);

                struct action a = {false};
                LOG_DBG("[Extension] sending start measurement with status %d", a.status);
                __zbus_chan_pub(__zbus_metadata_get_by_id(zbus_index_start_measurement),
                              (uint8_t *) &a, sizeof(a), K_MSEC(500), true);
            } break;
            default: {
                zassert_unreachable("Wrong idx send");
            }
            }
            memset(&msg_data, 0, sizeof(zbus_channel_variant_t));
        }
    }
}

void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_01));

    ztest_run_test_suite(framework_tests);
}

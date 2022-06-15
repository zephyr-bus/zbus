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


static void test_readonly_channel(void)
{
    struct version v = {0};
    ZBUS_CHAN_READ(version, v, K_NO_WAIT);
    zassert_equal(v.major, 0, "Major must be 0");
    zassert_equal(v.minor, 1, "Minor must be 1");
    zassert_equal(v.build, 1023, "Build must be 1023");
    struct zbus_channel *version_metadata = ZBUS_CHANNEL_GET(version);
    zassert_equal(version_metadata->flag.read_only, true, "It must be true");
    zassert_equal(version_metadata->message_size, sizeof(struct version),
                  "Message size is wrong");
    zassert_equal(version_metadata->flag.on_changed, false, NULL);
    zassert_equal(version_metadata->flag.read_only, true, NULL);
    zassert_equal(version_metadata->flag.on_changed, false, NULL);
    zassert_is_null(version_metadata->observers[0],
                    "There is no subscriber, it must be NULL");
}

void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_readonly_channel));

    ztest_run_test_suite(framework_tests);
}

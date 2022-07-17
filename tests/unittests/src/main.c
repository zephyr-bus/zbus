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

bool hard_msg_validator(void *msg, size_t msg_size)
{
    struct hard_msg *ref = (struct hard_msg *) msg;
    return (0 <= ref->range) && (ref->range <= 1023) && (ref->binary <= 1)
           && (ref->pointer != NULL);
}

static void test_hard_channel(void)
{
    struct hard_msg valid = {.range = 10, .binary = 1, .pointer = &valid.range};
    ZBUS_CHAN_PUB(hard_channel, valid, K_NO_WAIT);
    struct hard_msg current = {0};
    ZBUS_CHAN_READ(hard_channel, current, K_NO_WAIT);
    zassert_equal(valid.range, current.range, "Range must be equal");
    zassert_equal(valid.binary, current.binary, "Binary must be equal");
    zassert_equal(valid.pointer, current.pointer, "Pointer must be equal");
    struct hard_msg invalid = {.range = 10000, .binary = 1, .pointer = &valid.range};
    int err = zbus_chan_pub(ZBUS_CHAN_GET(hard_channel), (void *) &invalid,
                            sizeof invalid, K_NO_WAIT, false);
    zassert_true(err == -1, "Err must be -1, the channel message is invalid");
    invalid = (struct hard_msg) {.range=1, .binary=3, .pointer=&invalid.range};
    err = zbus_chan_pub(ZBUS_CHAN_GET(hard_channel), (void *) &invalid,
                            sizeof invalid, K_NO_WAIT, false);
    zassert_true(err == -1, "Err must be -1, the channel message is invalid");
    invalid = (struct hard_msg) {.range=1, .binary=0, .pointer=NULL};
    err = zbus_chan_pub(ZBUS_CHAN_GET(hard_channel), (void *) &invalid,
                        sizeof invalid, K_NO_WAIT, false);
    zassert_true(err == -1, "Err must be -1, the channel message is invalid");
}

static void test_readonly_channel(void)
{
    struct version v = {0};
    ZBUS_CHAN_READ(version, v, K_NO_WAIT);
    zassert_equal(v.major, 0, "Major must be 0");
    zassert_equal(v.minor, 1, "Minor must be 1");
    zassert_equal(v.build, 1023, "Build must be 1023");
    struct zbus_channel *version_metadata = ZBUS_CHAN_GET(version);
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
    ztest_test_suite(framework_tests, ztest_unit_test(test_readonly_channel),
                     ztest_unit_test(test_hard_channel));

    ztest_run_test_suite(framework_tests);
}

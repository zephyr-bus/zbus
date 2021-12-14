/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>
#include "zbus.h"

/*
 * @brief Systematic analyses of inputs
 name = str[0,1,>1]
 read_only, on_change = bool [false, true]
 message_type = [struct, union]
 subscribers = null ended array [0,1,>1]
 initial_value = [0, some, all]
 */


static void test_readonly_channel(void)
{
    struct version v = {0};
    zt_chan_read(version, v);
    zassert_equal(v.major, 0, "Major must be 0");
    zassert_equal(v.minor, 1, "Minor must be 1");
    zassert_equal(v.build, 1023, "Build must be 1023");
    zassert_equal(zt_chan_pub(version, v), -3,
                  "This channel is read only, it must not let anyone pub");

    struct metadata *version_metadata = ZT_CHANNEL_METADATA_GET(version);
    zassert_equal(strcmp(version_metadata->name, "version"), 0,
                  "Channel name must be 'version' and it is '%s'",
                  version_metadata->name);
    zassert_equal(version_metadata->channel_size, sizeof(struct version),
                  "Channel size is wrong");
    zassert_equal(version_metadata->flag.on_changed, false, NULL);
    zassert_equal(version_metadata->flag.read_only, true, NULL);
    zassert_equal(version_metadata->flag.on_changed, false, NULL);
    zassert_is_null(version_metadata->subscribers[0],
                    "There is no subscriber, it must be NULL");
}

void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_readonly_channel));

    ztest_run_test_suite(framework_tests);
}

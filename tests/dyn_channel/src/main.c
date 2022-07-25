/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <ztest.h>
#include "kernel.h"
#include "sys/util_macro.h"
#include "zbus.h"
#include "zbus_messages.h"
#include "ztest_assert.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

void s1_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(s1, s1_cb);


struct {
    uint8_t a;
    uint64_t b;
} my_random_data_expected = {0};

void s1_cb(zbus_chan_idx_t idx)
{
    struct external_data_msg *chan_message = NULL;
    zbus_chan_claim(zbus_chan_get_by_index(idx), (void *) &chan_message, K_NO_WAIT);
    memcpy(&my_random_data_expected, chan_message->reference,
           sizeof(my_random_data_expected));
    zbus_chan_finish(zbus_chan_get_by_index(idx));
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_static(void)
{
    uint8_t static_memory[256]                    = {0};
    struct external_data_msg static_external_data = {.reference = static_memory,
                                                     .size      = 256};

    struct {
        uint8_t a;
        uint64_t b;
    } my_random_data = {.a = 10, .b = 200000};
    memcpy(static_memory, &my_random_data, sizeof(my_random_data));

    int err = ZBUS_CHAN_PUB(dyn_chan, static_external_data, K_NO_WAIT);
    zassert_false(err, "Allocation could not be performed");

    k_msleep(100);
    zassert_equal(my_random_data.a, my_random_data_expected.a, "It must be 10");
    zassert_equal(my_random_data.b, my_random_data_expected.b, "It must be 200000");
    struct external_data_msg edm = {0};
    ZBUS_CHAN_READ(dyn_chan, edm, K_NO_WAIT);
    zassert_equal(edm.reference, static_memory, "The pointer must be equal here");
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_malloc(void)
{
    uint8_t *dynamic_memory = k_malloc(128);
    zassert_true(dynamic_memory != NULL, "Memory could not be allocated");
    struct external_data_msg static_external_data = {.reference = dynamic_memory,
                                                     .size      = 128};
    struct {
        uint8_t a;
        uint64_t b;
    } my_random_data = {.a = 20, .b = 300000};

    memcpy(dynamic_memory, &my_random_data, sizeof(my_random_data));
    int err = ZBUS_CHAN_PUB(dyn_chan, static_external_data, K_NO_WAIT);
    zassert_false(err, "Allocation could not be performed");

    k_msleep(100);
    zassert_equal(my_random_data.a, my_random_data_expected.a, "It must be 10");
    zassert_equal(my_random_data.b, my_random_data_expected.b, "It must be 200000");
    struct external_data_msg *actual_message_data = NULL;
    zbus_chan_claim(ZBUS_CHAN_GET(dyn_chan), (void **) &actual_message_data, K_NO_WAIT);


    zassert_equal(actual_message_data->reference, dynamic_memory,
                  "The pointer must be equal here");
    k_free(actual_message_data->reference);
    actual_message_data->reference = NULL;
    actual_message_data->size      = 0;
    zbus_chan_finish(ZBUS_CHAN_GET(dyn_chan));
    struct external_data_msg expected_to_be_clean = {0};
    ZBUS_CHAN_READ(dyn_chan, expected_to_be_clean, K_NO_WAIT);
    zassert_is_null(expected_to_be_clean.reference,
                    "The current message reference should be NULL");
    zassert_equal(expected_to_be_clean.size, 0,
                  "The current message size should be zero");
}

void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_static),
                     ztest_unit_test(test_malloc));

    ztest_run_test_suite(framework_tests);
}

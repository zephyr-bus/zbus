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
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

void s1_cb(zbus_channel_index_t idx);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s1, s1_cb);


struct {
    uint8_t a;
    uint64_t b;
} my_random_data_read = {0};

void s1_cb(zbus_channel_index_t idx)
{
    zbus_dyn_chan_read(__zbus_metadata_get_by_id(idx), (void *) &my_random_data_read,
                       sizeof(my_random_data_read), K_NO_WAIT);
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_static(void)
{
    uint8_t static_memory[256] = {0};
    int err =
        zbus_dyn_chan_alloc(ZBUS_CHANNEL_METADATA_GET(dyn_chan), (void *) static_memory,
                            sizeof(static_memory), K_NO_WAIT);
    zassert_false(err, "Allocation could not be performed");

    struct {
        uint8_t a;
        uint64_t b;
    } my_random_data = {.a = 10, .b = 200000};
    zbus_dyn_chan_pub(ZBUS_CHANNEL_METADATA_GET(dyn_chan), (void *) &my_random_data,
                      sizeof(my_random_data), K_NO_WAIT, false);
    k_msleep(100);
    zassert_equal(my_random_data.a, my_random_data_read.a, "It must be 10");
    zassert_equal(my_random_data.b, my_random_data_read.b, "It must be 200000");
    void *mem_ref = NULL;
    zbus_dyn_chan_dealloc(ZBUS_CHANNEL_METADATA_GET(dyn_chan), &mem_ref, K_NO_WAIT);
    zassert_equal(mem_ref, static_memory, "The pointer must be equal here");
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_malloc(void)
{
    uint8_t *memory = k_malloc(128);
    zassert_true(memory != NULL, "Memory could not be allocated");
    int err = zbus_dyn_chan_alloc(ZBUS_CHANNEL_METADATA_GET(dyn_chan), (void *) memory,
                                  128, K_NO_WAIT);
    zassert_false(err, "Allocation could not be performed");

    struct {
        uint8_t a;
        uint64_t b;
    } my_random_data = {.a = 20, .b = 300000};
    zbus_dyn_chan_pub(ZBUS_CHANNEL_METADATA_GET(dyn_chan), (void *) &my_random_data,
                      sizeof(my_random_data), K_NO_WAIT, false);
    k_msleep(100);
    zassert_equal(my_random_data.a, my_random_data_read.a, "It must be 10");
    zassert_equal(my_random_data.b, my_random_data_read.b, "It must be 200000");
    void *mem_ref = NULL;
    zbus_dyn_chan_dealloc(ZBUS_CHANNEL_METADATA_GET(dyn_chan), &mem_ref, K_NO_WAIT);
    k_free(memory);
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
static void test_borrow(void)
{
    struct {
        uint8_t a;
        uint64_t b;
    } my_random_data = {.a = 30, .b = 500000};

    LOG_DBG("Sizeof data %u", sizeof(my_random_data));
    uint8_t *memory = k_malloc(16);
    memset(memory, 0, 16);

    LOG_DBG("Alloc with data prt %p", memory);
    zassert_true(memory != NULL, "Memory could not be allocated");
    int err = zbus_dyn_chan_alloc(ZBUS_CHANNEL_METADATA_GET(dyn_chan), (void *) memory,
                                  16, K_NO_WAIT);
    zassert_true(err == 0, "Allocation could not be performed");

    void *memory_ref = NULL;
    err = zbus_dyn_chan_borrow(ZBUS_CHANNEL_METADATA_GET(dyn_chan), (void **) &memory_ref,
                               K_NO_WAIT);
    zassert_true(err == 0, "Could not perform the borrow");
    memcpy(memory_ref, &my_random_data, sizeof(my_random_data));
    zbus_dyn_chan_give_back(ZBUS_CHANNEL_METADATA_GET(dyn_chan), K_NO_WAIT);

    struct {
        uint8_t a;
        uint64_t b;
    } my_random_data_read2 = {0};
    zbus_dyn_chan_read(ZBUS_CHANNEL_METADATA_GET(dyn_chan),
                       (void *) &my_random_data_read2, sizeof(my_random_data_read),
                       K_NO_WAIT);
    zassert_equal(my_random_data.a, my_random_data_read2.a, "It must be 10");
    zassert_equal(my_random_data.b, my_random_data_read2.b, "It must be 200000");
    err = zbus_dyn_chan_dealloc(ZBUS_CHANNEL_METADATA_GET(dyn_chan), &memory_ref,
                                K_NO_WAIT);
    zassert_true(err == 0, "Could no reset the channel");
    k_free(memory_ref);
}
void test_main(void)
{
    ztest_test_suite(framework_tests, ztest_unit_test(test_static),
                     ztest_unit_test(test_malloc), ztest_unit_test(test_borrow));

    ztest_run_test_suite(framework_tests);
}

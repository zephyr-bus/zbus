/**
 * @file      zbus.h
 * @brief     Header of
 * @date      Tue Dec 14 14:19:05 2021
 * @author    Rodrigo Peixoto
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _ZBUS_H_
#define _ZBUS_H_

#include <string.h>
#include <zephyr.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "zbus_messages.h"


#ifndef ZBUS_CHANNEL
/**
 *
 * @brief Zbus channel definition.
 *
 * This macro defines a channel.
 *
 * @param name The channel's name.
 * @param on_changed Flag indicates if the subscribers of the channel would be notified
 * only by an actual message change. If a publishing action does not change the message
 * value it will not generate a notification event by the bus.
 * @param read_only Flag indicates the channel is read-only.
 * @param type The Message type. It must be a struct or union.
 * @param observer The observers list.
 * @see struct zbus_observer
 * @param init_val The message initialization.
 */
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val)
#endif

typedef enum __attribute__((packed)) {
#ifdef ZBUS_CHANNEL
#undef ZBUS_CHANNEL
#endif
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    name##_index,
#include "zbus_channels.h"
    ZBUS_CHANNEL_COUNT
} zbus_channel_index_t;

/**
 * @brief Type used to represent an observer.
 *
 * Every observer has an representation structure containing the relevant information.
 * An observer is a code portion interested in some channel. The observer can be notified
 * synchronously or asynchronously and it is called listener and subscriber respectively.
 * The observer can be enabled ou disabled during runtime by change the enabled boolean
 * field of the structure. The listeners have a callback function that is executed by the
 * bus with the index of the changed channel as argument when the notification is sent.
 * The subscribers have a message queue where the bus enqueues the index of the changed
 * channel when a notification is sent.
 *
 * @see zbus_observer_set_enable function to properly change the observer's enabled field.
 *
 */
struct zbus_observer {
    bool enabled;
    struct k_msgq *queue;
    void (*callback)(zbus_channel_index_t idx);
};

/**
 * @brief Type used to represent a channel.
 *
 * Every channel has a zbus_channel structure associated used to control the channel
 * access and usage.
 */
struct zbus_channel {
    struct {
        /** Pending callback flag. It is necessary to optimize two consecutive
         * publications. The ISR can cause this. Only the most recent of the enqueued
         * notification is actually performed. */
        uint8_t pend_callback : 1;
        /** On change flag. Indicates zbus will only consider sending a notification if
         * the last publication in this channel actually modified the message value,
         * otherwise zbus will ignore it.
         * */
        uint8_t on_changed : 1;
        /** Read only flag. Indicates the channel cannot receive publications. */
        uint8_t read_only : 1;
        /** Changes from extent. Indicates the channel was modified internally
         * by the extension feature and not by some user code. */
        uint8_t from_ext : 1;
    } flag;
    /** Lookup table index. Represents the index of the channel at the lookup table. */
    uint16_t lookup_table_index;
    /** Message size. Represents the channel's message size. */
    uint16_t message_size;
    /** Message reference. Represents the message's reference that points to the actual
     * shared memory region.
     */
    uint8_t *message;
    /** Message validator. Stores the reference to the function used by the publish action
     * to check the message before actually performing the it. No invalid messages can be
     * published. Every message is considered valid when this field is empty. */
    bool (*validator)(void *msg, size_t msg_size);
    /** Access control semaphore. Points to the semaphore used to avoid race conditions
     * for accessing the channel */
    struct k_sem *semaphore;
    /** Channel observer list. Represents the channel's observers list, it can be empty or
     * have listeners and subscribers mixed in any sequence */
    struct zbus_observer **observers;
};

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    type name;


/**
 * @brief Shared memory containing the message data
 *
 * @warning The user must not access this memory region directly.
 */
struct zbus_messages {
#include "zbus_channels.h"
};

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    struct zbus_channel _zbus_chan_##name;

/**
 * @brief Shared memory containing the channels data
 *
 * @warning The user must not access this memory region directly.
 */
struct zbus_channels {
#include "zbus_channels.h"
};

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    type name;

typedef union {
#include "zbus_channels.h"
} zbus_message_variant_t;


/* To avoid error when not using LOG */
#if defined(CONFIG_ZBUS_LOG)
#define ZBUS_LOG_DBG(...) LOG_DBG(__VA_ARGS__)
#else
#define ZBUS_LOG_DBG(...)
#endif

/**
 * @defgroup zbus_apis Zbus APIs
 * @ingroup datastructure_apis
 * @{
  FIX: Adjust this comment
 */

/**
 * @brief This function returns the __zbus_channels instance reference.
 *
 * Do not use this directly! It is being used by the auxiliary functions.
 *
 * @return A pointer of struct zbus_channels.
 */
struct zbus_messages *zbus_messages_instance();

/**
 * @brief This function returns the __zbus_channels instance reference.
 *
 * Do not use this directly! It is being used by the auxiliary functions.
 *
 * @return A pointer of struct zbus_channels.
 */
struct zbus_channels *zbus_channels_instance();

/**
 * @brief Retrieves the channel's metadata by a given index
 *
 * @param idx channel's index based on the generated enum.
 * @return the metadata struct of the channel
 */
struct zbus_channel *zbus_chan_get_by_index(zbus_channel_index_t idx);

#if defined(CONFIG_ZBUS_ASSERTS)
/**
 *
 * @brief Zbus assert.
 *
 * This macro checks the condition stopping the execution by a k_oops if the condition is
 * not true.
 *
 * @param cond The condition to be checked.
 */
#define ZBUS_ASSERT(cond)                                                                \
    do {                                                                                 \
        if (!(cond)) {                                                                   \
            printk("Assertion failed %s:%d(%s): %s\n", __FILE__, __LINE__, __FUNCTION__, \
                   #cond);                                                               \
            k_oops();                                                                    \
        }                                                                                \
    } while (0)
#else
#define ZBUS_ASSERT(cond)
#endif

/**
 *
 * @brief Initialize a message.
 *
 * This macro initializes a message by passing the values to initialize the message struct
 * or union.
 *
 * @param[in] val Variadic with the initial values.
 */
#define ZBUS_MSG_INIT(val, ...) \
    {                           \
        val, ##__VA_ARGS__      \
    }

/**
 *
 * @brief Define and initialize a subscriber.
 *
 * This macro establishes the message queue where the subscriber will receive the
 * notification asynchronously, and initialize the struct defining the subscriber.
 *
 * @param[in] name The subscriber's name.
 * @param[in] queue_size The notification queue's size.
 */
#define ZBUS_SUBSCRIBER_DECLARE(name, queue_size)                         \
    K_MSGQ_DEFINE(name##_queue, sizeof(zbus_channel_index_t), queue_size, \
                  sizeof(zbus_channel_index_t));                          \
    struct zbus_observer name = {                                         \
        .enabled  = true,                                                 \
        .queue    = &name##_queue,                                        \
        .callback = NULL,                                                 \
    }

/**
 *
 * @brief Define and initialize a listener.
 *
 * This macro establishes the callback where the listener will be notified synchronously,
 * and initialize the struct defining the listener.
 *
 * @param[in] name The listener's name.
 * @param[in] cb The callback function.
 */
#define ZBUS_LISTENER_DECLARE(name, cb) \
    struct zbus_observer name = {       \
        .enabled  = true,               \
        .queue    = NULL,               \
        .callback = (cb),                 \
    }

/**
 *
 * @brief Get a channel metadata.
 *
 * This macro gets a channel's metadata using the channel's name.
 *
 * @param[in] chan The channel's name.
 *
 * @return The channel's metadata.
 */
#define ZBUS_CHAN_GET(chan) \
    ((struct zbus_channel *) &zbus_channels_instance()->_zbus_chan_##chan)

/**
 *
 * @brief Get a message data.
 *
 * This macro gets a channel's message using the channel's name.
 *
 * @param[in] chan The channel's name.
 *
 * @return The channel's message.
 */
#define ZBUS_MSG_GET(chan) (((struct zbus_messages *) zbus_messages_instance())->chan)


/**
 *
 * @brief Publish to a channel
 *
 * This macro publishes a message to a channel using the channel's name.
 *
 * @param[in] chan The channel's name.
 * @param[out] value Message where the publish function copies the channel's
 * message data from.
 * @param[in] timeout Waiting period to read the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel is published.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
#define ZBUS_CHAN_PUB(chan, value, timeout)                                              \
    ({                                                                                   \
        {                                                                                \
            __typeof__(ZBUS_MSG_GET(chan)) chan##__aux__;                                \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        ZBUS_LOG_DBG("[ZBUS] %spub " #chan " at %s:%d", (k_is_in_isr() ? "ISR " : ""), \
                       __FILE__, __LINE__);                                              \
        zbus_chan_pub(ZBUS_CHAN_GET(chan), (uint8_t *) &(value), sizeof(value), timeout,   \
                      false);                                                            \
    })

/**
 *
 * @brief Publish to a channel
 *
 * This macro publishes a message to a channel using the channel's index.
 *
 * @param[in] chan The channel's name.
 * @param[out] value Message where the publish function copies the channel's
 * message data from.
 * @param[in] timeout Waiting period to read the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel is published.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
#define ZBUS_CHAN_PUB_BY_INDEX(idx, value, timeout)                                    \
    ({                                                                                 \
        ZBUS_LOG_DBG("[ZBUS] %spub %d at %s:%d", (k_is_in_isr() ? "ISR " : ""), idx, \
                       __FILE__, __LINE__);                                            \
        zbus_chan_pub(zbus_chan_get_by_index(idx), (uint8_t *) &(value),                 \
                      zbus_chan_get_by_index(idx)->message_size, timeout, false);      \
    })

/**
 *
 * @brief Publish to a channel
 *
 * This routine publishes a message to a channel.
 *
 * @param meta The channel's metadata.
 * @param msg Reference to the message where the publish function copies the channel's
 * message data from.
 * @param msg_size Size of the message to be publish.
 * @param timeout Waiting period to publish the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel is published.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
int zbus_chan_pub(struct zbus_channel *meta, uint8_t *msg, size_t msg_size,
                  k_timeout_t timeout, bool from_ext);


/**
 *
 * @brief Read a channel
 *
 * This macro reads a message from a channel.
 *
 * @param[in] chan The channel's name.
 * @param[out] value Message where the read function copies the channel's
 * message data to.
 * @param[in] timeout Waiting period to read the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel is read.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
#define ZBUS_CHAN_READ(chan, value, timeout)                                             \
    ({                                                                                   \
        {                                                                                \
            __typeof__(ZBUS_MSG_GET(chan)) chan##__aux__;                                \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        ZBUS_LOG_DBG("[ZBUS] %sread " #chan " at %s:%d",                               \
                       (k_is_in_isr() ? "ISR " : ""), __FILE__, __LINE__);               \
        zbus_chan_read(ZBUS_CHAN_GET(chan), (uint8_t *) &(value), sizeof(value), timeout); \
    })

/**
 *
 * @brief Read a channel using the index
 *
 * This macro reads a message from a channel by using the channel's index.
 *
 * @param[in] idx The channel's global index defined by zbus.
 * @param[out] value Message where the read function copies the channel's
 * message data to.
 * @param[in] timeout Waiting period to read the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel is read.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
#define ZBUS_CHAN_READ_BY_INDEX(idx, value, timeout)                                    \
    ({                                                                                  \
        ZBUS_LOG_DBG("[ZBUS] %sread %d at %s:%d", (k_is_in_isr() ? "ISR " : ""), idx, \
                       __FILE__, __LINE__);                                             \
        zbus_chan_read(zbus_chan_get_by_index(idx), (uint8_t *) &(value),                 \
                       zbus_chan_get_by_index(idx)->message_size, timeout);             \
    })

/**
 *
 * @brief Read a channel
 *
 * This routine reads a message from a channel.
 *
 * @param meta The channel's metadata.
 * @param msg Reference to the message where the read function copies the channel's
 * message data to.
 * @param msg_size Size of the message to be read.
 * @param timeout Waiting period to read the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel is read.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
int zbus_chan_read(struct zbus_channel *meta, uint8_t *msg, size_t msg_size,
                   k_timeout_t timeout);


/**
 *
 * @brief Get address of the channel's message.
 *
 * With this routine, the developer can access the channel's message data directly.
 *
 * @warning After calling this routine, the channel cannot be used by other
 * thread until the zbus_chan_finish routine is performed.
 *
 * @warning This routine should only be called once before a zbus_chan_finish.
 *
 * @param[in] meta The channel's metadata.
 * @param[out] msg Data pointer to the message's address. It is set to the actual message
 * address.
 * @param[in] timeout Waiting period to claim the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel claimed.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
int zbus_chan_claim(struct zbus_channel *meta, void **chan_msg, k_timeout_t timeout);

/**
 *
 * @brief Indicate the claimed message is being released.
 *
 * After calling this routine with success, the channel will be able to be used by other
 * thread.
 *
 * @warning This routine must only be used after a zbus_chan_claim.
 *
 * @param meta The channel's metadata.
 * @param msg Pointer to the message's data reference.
 * @param timeout Waiting period to claim the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel claimed.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
void zbus_chan_finish(struct zbus_channel *meta, k_timeout_t timeout);

/**
 *
 * @brief Force a notification to the subscribers of a channel.
 *
 * Indicates to zbus to forcefully notify the subscribers of a channel.
 *
 * @param meta The channel's metadata.
 * @param timeout Waiting period to claim the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel notified.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
int zbus_chan_notify(struct zbus_channel *meta, k_timeout_t timeout);

/**
 *
 * @brief Print the channel's summary.
 *
 * Prints the summary of the defined channels in json format. This routine would be used
 * by one who wants to extend the bus.
 *
 * @param meta The channel's metadata.
 * @param timeout Waiting period to claim the channel,
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @retval 0 channel claimed.
 * @retval -ETIMEDOUT Waiting period timed out.
 * @retval -EINVAL Some parameter is invalid.
 */
void zbus_info_dump(void);

/**
 *
 * @brief Change the observer state.
 *
 * This routine changes the observer state.
 *
 * @param[in] sub The observer's reference.
 * @param[in] enabled State to be. When false the observer stops to receive notifications.
 */
void zbus_observer_set_enable(struct zbus_observer *sub, bool enabled);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _ZBUS_H_ */

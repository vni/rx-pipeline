/**
 * @file observable.h
 * @date 16 Dec 2016
 * @brief Observable pipe common interface
 */

#pragma once

#include "common.h"

struct Observable;

/** @addtogroup Observable_Interface
 *  @{
 */

/**
 * @brief Observable typedef to calm down doxygen
 */
typedef struct Observable Observable;

/**
 * @brief Callback to handle Observable event data.
 *        Incoming data may be data buffer, or 'buffer_end_of_data()'.
 *
 *        If you want to indicate end of observable data, return 'buffer_end_of_data()'.
 *        If you don't have data to broadcast, return 'buffer_no_data()'.
 */
typedef Buffer(*observable_cb)(Observable *observable, Buffer data);

/**
 * @brief Callback to destroy observable (virtual destructor)
 */
typedef void (*observable_destroy_cb)(Observable *observable);

/**
 * @brief Observable pipe
 */
struct Observable {
    khash_t(ptr_hash_map) *subscribers; /**< Set of observable subscribers */
    observable_cb callback;             /**< Callback to cal on event */
    observable_destroy_cb destroy_cb;   /**< Callback to call to free internal resources */
    void *data;                         /**< User data */
};

/**
 * @brief observable_init Initialize observable internal data
 * @param observable Observable
 */
void observable_init(Observable *observable);

/**
 * @brief observable_deinit Free observable internal data
 * @param observable Observable
 */
void observable_deinit(Observable *observable);

/**
 * @brief observable_broadcast Broadcast data to all observable subscribers
 * @param observable Observable
 * @param data Data to broadcast
 */
void observable_broadcast(Observable *observable, Buffer data);

/**
 * @brief observable_subscribe Subscribe to observable events
 * @param listener Observable subscribe to
 * @param subscriber Subscriber
 */
void observable_subscribe(Observable *listener, Observable *subscriber);

/**
 * @brief observable_unsubscribe Unsubscribe from following events
 * @param listener Observable unsubscribe from
 * @param subscriber Subscriber to unsubscribe
 */
void observable_unsubscribe(Observable *listener, Observable *subscriber);

/**
 * @brief observable_destroy Destroy observable. Unsubscribe from all
 *        observables and free internal resources. If observable still
 *        has subscribers, does nothing
 * @param observable Observable to destroy
 * @return If successfully destroyed.
 */
bool observable_destroy(Observable *observable);

/** @}*/

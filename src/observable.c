
#include "observable.h"
#include "utils.h"

// Eond of data
static void* EOD = GINT_TO_POINTER(0xE0D);

void *end_of_data() {
    return EOD;
}

void observable_init(Observable *observable) {
    if (observable) {
        observable->subscribers = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                  NULL,
                                  NULL);
        observable->destroy_cb = NULL;
        observable->data = NULL;
    }
}

void observable_deinit(Observable *observable) {
    g_hash_table_destroy(observable->subscribers);
}

static void send_to_subscriber(/*gpointer key,*/ gpointer value, gpointer user_data) {
    Observable *obs = (Observable *) value;

    if (obs->callback && user_data) {
        // If result of our callback non-NULL, broadcast to subscribers.
        void *result = obs->callback(obs, user_data);

        if (result) {
            observable_broadcast(obs, result);
        }
    }
}

void observable_broadcast(Observable *observable, void *data) {
    if (data) {
        // We may destroy observive inside a callback and this changes subscriber
        // hashtable layout, so to iterate we have to use list of keys
        GList *subscribers = g_hash_table_get_keys(observable->subscribers);
        g_list_foreach(subscribers, send_to_subscriber, data);
        g_list_free(subscribers);
    }
}

void observable_subscribe(Observable *listener, Observable *subscriber) {
    g_hash_table_add(listener->subscribers, subscriber);
}

void observable_unsubscribe(Observable *listener, Observable *subscriber) {
    g_hash_table_remove(listener->subscribers, subscriber);

    if (g_hash_table_size(listener->subscribers) == 0) {
        observable_destroy(listener);
    }
}

bool observable_destroy(Observable *observable) {
    CHECK_NULL_RETURN(observable, false);

    if (g_hash_table_size(observable->subscribers) == 0) {
        observable->destroy_cb(observable);
        return true;
    }

    return false;
}



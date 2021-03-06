#include "observable_map.h"


/**
 * @brief Observable map
 * @ingroup Observable_Map
 */
typedef struct {
    Observable base;    /**< Parent observable structure */
    Observable *source; /**< Source Observable to map */
} Map;

static void map_destroy_callback(Observable *observable) {
    if (kh_size(observable->subscribers) == 0) {
        observable_deinit(observable);

        Map *map = (Map *) observable;

        if (map->source) {
            observable_unsubscribe(map->source, observable);
        }

        free(map);
    }
}

Observable *observable_map_create(Observable *observable, observable_cb callback) {
    CHECK_NULL_RETURN(observable, NULL);

    Map *result = xmalloc(sizeof(Map));

    observable_init(&result->base);
    result->base.destroy_cb = map_destroy_callback;
    result->source = observable;
    result->base.callback = callback;
    observable_subscribe(observable, (Observable *) result);

    return (Observable *) result;
}

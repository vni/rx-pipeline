#include "observable_data_generator.h"

/**
 * @brief Generator. Generates observable data
 */
typedef struct {
    Observable base;
    generator_fn func;
    bool run;
} Generator;

static void generator_destroy_cb(Observable *observable) {
    Generator *generator = (Generator *) observable;
    generator->run = false;
}

Observable *observable_generator_create(generator_fn func) {
    Generator *result = malloc(sizeof(Generator));

    observable_init(&result->base);
    result->base.destroy_cb = generator_destroy_cb;
    result->func = func;
    result->run = true;

    return (Observable *) result;
}

void observable_generator_run(Observable *observable) {
    Generator *generator = (Generator *) observable;

    while (generator->run) {
        observable_broadcast(&generator->base, generator->func());
    }

    observable_deinit(&generator->base);
    free(generator);
}


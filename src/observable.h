#pragma once

#include "atom.h"

struct Observable;
typedef struct Observable Observable;

typedef void *(*observable_cb)(Observable *observable, void *data);
typedef void (*observable_destroy_cb)(Observable *observable);

#include "observable_join.h"
#include "observable_timer.h"
#include "observable_pipe.h"
#include "pipeline.h"

struct Observable {
    GHashTable *subscribers;
    observable_cb callback;
    observable_destroy_cb destroy_cb;
    void *data;
};

void observable_init(Observable *observable);
void observable_broadcast(Observable *observable, void *data);
bool observable_destroy(Observable *observable);

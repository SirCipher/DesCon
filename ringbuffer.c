#include <stdlib.h>
#include <assert.h>
#include "ringbuffer.h"

struct ringbuffer_t {
    BUFFERTYPE *buffer, *head, *tail, *end;
    size_t count;
    size_t size;
};

ringbuffer_t ringbuffer_new(size_t size) {
    ringbuffer_t rbuffer = NULL;

    rbuffer = malloc(sizeof(struct ringbuffer_t));

    assert(rbuffer != NULL);

    rbuffer->size = size;
    rbuffer->buffer = malloc((rbuffer->size) * sizeof(BUFFERTYPE));
    assert(rbuffer->buffer);

    rbuffer->head = rbuffer->tail = rbuffer->buffer;
    rbuffer->count = 0;
    rbuffer->end = rbuffer->buffer + rbuffer->size;

    return rbuffer;
};


void ringbuffer_free(ringbuffer_t *buffer) {
    assert(buffer && *buffer);
    free((*buffer)->buffer);
    free(*buffer);
    *buffer = NULL;
}

size_t ringbuffer_size(ringbuffer_t buffer) {
    return buffer->size;
}

BUFFERTYPE ringbuffer_head(ringbuffer_t buffer) {
    return *buffer->head;
}

BUFFERTYPE ringbuffer_tail(ringbuffer_t buffer) {
    return *buffer->tail;
}

size_t ringbuffer_count(ringbuffer_t buffer) {
    return buffer->count;
}

int ringbuffer_push(ringbuffer_t buffer, BUFFERTYPE item) {
    if (ringbuffer_is_full(buffer)) {
        return 0;
    }
    *buffer->tail++ = item;
    if (buffer->tail >= buffer->end) {
        buffer->tail = buffer->buffer;
    }
    buffer->count++;
    return 1;
}

BUFFERTYPE ringbuffer_shift(ringbuffer_t buffer) {
    if (ringbuffer_is_empty(buffer)) {
        return NULL;
    }
    buffer->count--;
    BUFFERTYPE output = *buffer->head++;
    if (buffer->head >= buffer->end) {
        buffer->head = buffer->buffer;
    }
    return output;
}

int ringbuffer_is_full(ringbuffer_t buffer) {
    return buffer->count == buffer->size;
}

int ringbuffer_is_empty(ringbuffer_t buffer) {
    return buffer->count == 0;
}



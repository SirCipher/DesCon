
//
// Created by Adam on 19/02/2017.
//

#ifndef RINGBUFFER_RING_BUFFER_H
#define RINGBUFFER_RING_BUFFER_H
#include <stdlib.h>
#include <assert.h>
// Allows us to change the buffer type easily
#define BUFFERTYPE char

typedef struct ringbuffer_t *ringbuffer_t;

/*
 * Create a new ring buffer that has the given size, (size is maximum number of BUFFERTYPE it can hold)
 */
ringbuffer_t ringbuffer_new(size_t size);

/*
 * Deletes a ringbuffer and frees up the memory it was using
 */
void ringbuffer_free(ringbuffer_t *buffer);


/*
 * retrieves the maximum size of the buffer
 */
size_t ringbuffer_size(ringbuffer_t buffer);
/*
 * retrieves the head of the buffer
 */
BUFFERTYPE ringbuffer_head(ringbuffer_t buffer);
/*
 * retrieves the tail of the buffer
 */
BUFFERTYPE ringbuffer_tail(ringbuffer_t buffer);

/*
 * Counts the actual number of items in the buffer
 */
size_t ringbuffer_count(ringbuffer_t buffer);


/*
 * Adds an item to the buffer
 */
int ringbuffer_push(ringbuffer_t buffer, BUFFERTYPE item);
/*
 * Removes an item form the buffer and returns it
 */
BUFFERTYPE ringbuffer_shift(ringbuffer_t buffer);


/*
 * Returns whether the buffer
 */
int ringbuffer_is_full(ringbuffer_t buffer);

/*
 * Returns whether the buffer is empty
 */
int ringbuffer_is_empty(ringbuffer_t buffer);


#endif //RINGBUFFER_RING_BUFFER_H

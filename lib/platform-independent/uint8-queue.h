#ifndef UINT8_QUEUE_H
#define UINT8_QUEUE_H

#include <stdint.h>

typedef struct {
    uint8_t *data = nullptr;
    uint8_t size = 0;
    uint8_t front = 0;
    uint8_t rear = 0;
    bool has_overflowed = false;
} uint8_queue_t;

typedef struct
{
    uint8_t value;
    bool is_valid;
} dequeue_return_t;

void init_queue(uint8_queue_t *queue, uint8_t *buffer, uint8_t buffer_size);
void add_to_queue(uint8_queue_t *queue, uint8_t value);
dequeue_return_t dequeue(uint8_queue_t *queue);
bool has_queue_overflowed(uint8_queue_t *queue);

bool queue_is_empty(uint8_queue_t *queue);
bool queue_is_full(uint8_queue_t *queue);

#endif //UINT8_QUEUE

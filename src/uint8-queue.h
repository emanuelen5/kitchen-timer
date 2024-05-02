#ifndef UINT8_QUEUE_H
#define UINT8_QUEUE_H

#include <stdint.h>

#define QUEUE_SIZE 10

typedef struct {
    uint8_t data[QUEUE_SIZE];
    int front = 0;
    int rear = 0;
} uint8_queue_t;

typedef struct
{
    uint8_t value;
    bool is_valid;
} dequeue_return_t;

void init_queue(uint8_queue_t *queue);
void add_to_queue(uint8_queue_t *queue, uint8_t value);
dequeue_return_t dequeue(uint8_queue_t *queue);

bool queue_is_empty(uint8_queue_t *queue);
bool queue_is_full(uint8_queue_t *queue);

#endif //UINT8_QUEUE

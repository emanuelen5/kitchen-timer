
#include "uint8-queue.h"

void init_queue(uint8_queue_t *queue, uint8_t *buffer, uint8_t buffer_size)
{
    queue->data = buffer;
    queue->size = buffer_size;
    queue->front = 0;
    queue->rear = 0;
}

void add_to_queue(uint8_queue_t *queue, uint8_t value)
{
    if (queue_is_full(queue))
    {
        queue->has_overflowed = true;
        return;
    }

    queue->data[queue->rear] = value;
    queue->rear = (queue->rear + 1) % queue->size;
}

dequeue_return_t dequeue(uint8_queue_t *queue)
{
    dequeue_return_t v = {};
    if (queue_is_empty(queue))
    {
        v.is_valid = false;
        return v;
    }

    v.value = queue->data[queue->front];
    v.is_valid = true;
    queue->front = (queue->front + 1) % queue->size;
    return v;
}

bool has_queue_overflowed(uint8_queue_t *queue)
{
    bool o = queue->has_overflowed;
    queue->has_overflowed = false;
    return o;
}

bool queue_is_empty(uint8_queue_t *queue)
{
    return queue->front == queue->rear;
}

bool queue_is_full(uint8_queue_t *queue)
{
    return queue->front == (queue->rear + 1) % queue->size;
}


#include "uint8-queue.h"

void init_queue(uint8_queue_t *queue)
{
    queue->front = 0;
    queue->rear = 0;
}

void add_to_queue(uint8_queue_t *queue, uint8_t value)
{
    if (queue_is_full(queue))
        return;  // TODO: Error handling

    queue->data[queue->rear] = value;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
}

dequeue_return_t dequeue(uint8_queue_t *queue)
{
    dequeue_return_t v = {0};
    if (queue_is_empty(queue))
    {
        v.is_valid = false;
        return v;
    }

    v.value = queue->data[queue->front];
    v.is_valid = true;
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    return v;
}

bool queue_is_empty(uint8_queue_t *queue)
{
    return queue->front == queue->rear;
}

bool queue_is_full(uint8_queue_t *queue)
{
    return queue->front == (queue->rear + 1) % QUEUE_SIZE;
}

#include "event_queue.h"



static eventHandlerFunction handleEvent;

void init_event_queue(event_queue_t *queue, eventHandlerFunction eventFunction)
{
    handleEvent = eventFunction;
    queue -> front = 0;
    queue -> rear = 0;
}

void queue_event(event_queue_t *queue, uint8_t event)
{
    if (event_queue_is_full(queue))
        return;  // TODO: Error handling
        
    queue -> data[queue -> rear] = event;
    queue -> rear = (queue -> rear + 1) % QUEUE_SIZE;
}

void dequeue_event(event_queue_t *queue) 
{
    if (event_queue_is_empty(queue))
        return;
        
    handleEvent(queue ->data[queue -> front]);
    queue -> front = (queue -> front + 1) % QUEUE_SIZE;
}

bool event_queue_is_empty(event_queue_t *queue)
{
    return queue -> front == queue -> rear;
}

bool event_queue_is_full(event_queue_t *queue)
{
    return queue -> front == (queue -> rear + 1) % QUEUE_SIZE;
}

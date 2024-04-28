#include "event_queue.h"

event_queue_t eventQueue;

static eventHandlerFunction handlingEvent;

void init_event_queue(eventHandlerFunction eventFunction)
{
    handlingEvent = eventFunction;
}

void queuing_event(event_t event)
{
    if (event_queue_is_full(queue))
        return;  // TODO: Error handling
        
    queue.data[queue.rear] = event;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
}

void dequeuing_event()
{
    if (event_queue_is_empty(&eventQueue))
    {
        return;
    }
    else
    {
        handlingEvent(eventQueue.data[eventQueue.front]);
        eventQueue.front = (eventQueue.front + 1) % QUEUE_SIZE;
    }
}

bool event_queue_is_empty(event_queue_t *queue)
{
    return queue -> front == queue -> rear;
}

bool event_queue_is_full(event_queue_t *queue)
{
    return queue -> front == (queue -> rear + 1) % QUEUE_SIZE;
}

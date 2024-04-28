
#include "event_queue.h"

event_queue_t eventQueue;

static eventHandlerFunction handleEvent;

void init_event_queue(eventHandlerFunction eventFunction)
{
    handleEvent = eventFunction;
    eventQueue.front = 0;
    eventQueue.rear = 0;
}

void queue_event(uint8_t event)
{
    if (event_queue_is_full(&eventQueue))
    {
        // Code for handling overflow
    } 
    else 
    {
        eventQueue.data[eventQueue.rear] = event;
        eventQueue.rear = (eventQueue.rear + 1) % QUEUE_SIZE;
    }
}

void dequeue_event()
{
    if (event_queue_is_empty(&eventQueue))
    {
        return;
    }
    else
    {
        handleEvent(eventQueue.data[eventQueue.front]);
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

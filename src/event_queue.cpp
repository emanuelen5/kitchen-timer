#include "event_queue.h"

event_queue_t eventQueue;

static eventHandlerFunction handlingEvent;

void init_event_queue(eventHandlerFunction eventFunction)
{
    handlingEvent = eventFunction;
}

void queuing_event(event_t event)
{
    if (!event_queue_is_empty(&eventQueue, 1))     //evaluates if queue is full
    {
        eventQueue.data[eventQueue.rear] = event;
        eventQueue.rear = (eventQueue.rear + 1) % QUEUE_SIZE;
    } 
    else 
    {
        // Code for handling overflow
    }
}

void dequeuing_event()
{
    if (event_queue_is_empty(&eventQueue, 0))
    {
        return;
    }
    else
    {
        handlingEvent(eventQueue.data[eventQueue.front]);
        eventQueue.front = (eventQueue.front + 1) % QUEUE_SIZE;
    }
}

bool event_queue_is_empty(event_queue_t *queue, int extra_step)
{
    return queue -> front == (queue -> rear + extra_step) % QUEUE_SIZE;
}

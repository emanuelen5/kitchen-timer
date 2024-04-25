#include "event_queue.h"

event_queue_t eventQueue;

static event_function handlingEvent;

void init_event_queue(event_function executingEvent)
{
    handlingEvent = executingEvent;
}

void queuing_event(event_t event)
{
    // Check if the queue is full
    if ((eventQueue.rear + 1) % QUEUE_SIZE != eventQueue.front) {
        // Add the interrupt to the queue
        eventQueue.data[eventQueue.rear] = event;
        eventQueue.rear = (eventQueue.rear + 1) % QUEUE_SIZE;
    } else {
        // Handle queue overflow (optional)
    }
}

void dequeuing_event()
{
    if (event_queue_is_empty(&eventQueue)) {
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

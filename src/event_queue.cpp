#include "event_queue.h"

void queuing_event(int event)
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
        // Process the interrupt
        switch (eventQueue.data[eventQueue.front])
        {
            case CW_INTERRUPT:
                cw_rotation();
                break;

            case CCW_INTERRUPT:
                ccw_rotation();
                break;
        }
        //Move the front marker
        eventQueue.front = (eventQueue.front + 1) % QUEUE_SIZE;
    }
}

bool event_queue_is_empty(event_queue_t *queue)
{
    return queue -> front == queue -> rear;
}

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#define QUEUE_SIZE 10

typedef enum event
{
    PRESS,
    CW_ROTATION,
    CCW_ROTATION,
    DOUBLE_PRESS,
    LONG_PRESS,
    CW_PRESSED_ROTATION,
    CCW_PRESSED_ROTATION,
    TIMEOUT,
    SECOND_TICK,
} event_t;

typedef struct {
    event_t data[QUEUE_SIZE];
    int front = 0;
    int rear = 0;
} event_queue_t;

typedef void (*eventHandlerFunction)(int);

void init_event_queue(eventHandlerFunction eventFunction);
void queuing_event(event_t event);
void dequeuing_event();
bool event_queue_is_empty(event_queue_t *queue);
bool event_queue_is_full(event_queue_t *queue)

#endif

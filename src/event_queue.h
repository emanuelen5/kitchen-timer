#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H


#define QUEUE_SIZE 10

typedef struct {
    event_t data[QUEUE_SIZE];
    int front = 0;
    int rear = 0;
} event_queue_t;

event_queue_t eventQueue;

void queuing_event(int event);
void dequeuing_event();
bool event_queue_is_empty(event_queue_t *queue);


#endif

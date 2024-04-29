#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <stdint.h>

#define QUEUE_SIZE 10

typedef struct {
    uint8_t data[QUEUE_SIZE];
    int front = 0;
    int rear = 0;
} event_queue_t;

typedef void (*eventHandlerFunction)(int);

void init_event_queue(event_queue_t *queue, eventHandlerFunction eventFunction);
void queue_event(event_queue_t *queue, uint8_t event);
void dequeue_event(event_queue_t *queue);
bool event_queue_is_empty(event_queue_t *queue);
bool event_queue_is_full(event_queue_t *queue);

#endif

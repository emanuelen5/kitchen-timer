#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include "util.h"
#include "uint8-queue.h"

#define CS_PIN PB0
#define CLK_PIN PB5
#define DATA_PIN PB3

void init_SPI(uint8_t bytes);
void SPI_transmit_byte(uint8_t byte);
void add_to_SPI_queue(uint8_t value);
dequeue_return_t dequeue_from_SPI_queue(void);
void start_SPI_transfer();


#endif // SPI_H

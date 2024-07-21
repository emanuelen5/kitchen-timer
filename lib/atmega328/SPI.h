#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

void SPI_Init(uint8_t cs_pin);
void SPI_transmit_byte(uint8_t byte);

#endif // SPI_H

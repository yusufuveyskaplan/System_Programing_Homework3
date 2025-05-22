#ifndef LSBSTEGAN_H
#define LSBSTEGAN_H

#include <stdint.h>
#include "image.h"


int encode_message(Image *img, const char *message, uint8_t nbit_per_channel);
uint8_t set_lsb(uint8_t byte, uint8_t bits, uint8_t nbit_per_channel);
uint8_t get_lsb(uint8_t byte, uint8_t nbit_per_channel);
char *decode_message(const Image *img, uint8_t nbit_per_channel);

#endif

#ifndef ENCODING_FUNCTIONS_H
#define ENCODING_FUNCTIONS_H

#include <stdbool.h>

unsigned short hamming_encode_12_8(char data);
unsigned int encode_pixel(unsigned int pixel_bytes, char letter, bool verbose);
void add_meta(char* message, unsigned char num_meta_bytes, char* meta_bytes);
void encode_image(unsigned int message_size, unsigned char* img, char* message, bool verbose);


#endif
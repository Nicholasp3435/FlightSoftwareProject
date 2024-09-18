#include "encoding_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned int encode_pixel(unsigned int pixel_bytes, char letter, bool verbose) {
    if (verbose) {
        printf("Encoding %c into #%x:\t", letter, pixel_bytes);
    }

    unsigned int encoded_pixel = 0;
    unsigned int bit_mask = 0xFCFCFCFC;

    // bit masks for each channel
    pixel_bytes &= bit_mask;

    // encoding letter1
    unsigned char letter_mask = 0b11000000;
    unsigned int split_letter = 0;

    for (int i = 3; i >= 0; i--) {
        unsigned int current_channel = letter_mask & letter;
        current_channel = current_channel << (6 * i);
        split_letter |= current_channel;
        letter_mask >>= 2;
    }


    // if compression_level == 1, only encode letter1
    encoded_pixel = pixel_bytes | split_letter;


    if (verbose) {
        printf("#%x\n", encoded_pixel);
    }

    return encoded_pixel;
}

void encode_image(unsigned int message_size, unsigned char* img, char* message, bool verbose) {
    for (unsigned int i = 0; i < message_size; i++) {
        unsigned int pixel_bytes = 0;

        // puts the channels of the pixels into an int for encoding
        for (char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        }

        // encode based on compression level
        unsigned int encoded_pixel = encode_pixel(pixel_bytes, message[i], verbose);
        

        // places the encoded pixel in the image
        for (char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            img[pixel_index] = (encoded_pixel >> (3 - j) * 8);
        }
    }
}


void add_meta(char* message, unsigned char num_meta_bytes, char* meta_bytes) {
    for (unsigned char i = 0; i < num_meta_bytes; i++) {
        message[i] = meta_bytes[i];
    }
}
#include "encoding_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


unsigned short steamed_hams(char letter) {
    unsigned short encoded = 0;

    unsigned short g1, g2, g3 = 0;

    // splits letter into groups
    g1 = letter & 0b10000000;
    g2 = letter & 0b01110000;
    g3 = letter & 0b00001111;

    g1 <<= 2;
    g2 <<= 1;

    // puts the groups into the respective bits
    encoded = (g1 | g2 | g3);

    unsigned short p1, p2, p4, p8 = 0;

    // hamming(12, 8) layout: p1 p2 d1 p4 d2 d3 d4 p8 d5 d6 d7 d8

    p1 = ((encoded >> 9) ^ (encoded >> 7) ^ (encoded >> 5) ^ (encoded >> 3) ^ (encoded >> 1)) & 1;
    p2 = ((encoded >> 9) ^ (encoded >> 6) ^ (encoded >> 5) ^ (encoded >> 2) ^ (encoded >> 1)) & 1;
    p4 = ((encoded >> 7) ^ (encoded >> 6) ^ (encoded >> 5)) & 1;
    p8 = ((encoded >> 3) ^ (encoded >> 2) ^ (encoded >> 1)) & 1;

    p1 <<= 11;
    p2 <<= 10;
    p4 <<= 8;
    p8 <<= 4;

    // puts the parity bits into the respective bits
    encoded |= (p1 | p2 | p4 | p8);

    return encoded;
}


unsigned int encode_pixel(unsigned int pixel_bytes, char letter, bool verbose) {
    if (verbose) {
        printf("Encoding %c into #%x:\t", letter, pixel_bytes);
    }

    unsigned int bit_mask = 0xF8F8F8F8;  // Clear the 3 LSBs for each channel (4 channels = 4 * 3 = 12 bits)

    // Apply bit mask to clear 3 LSBs of each channel
    pixel_bytes &= bit_mask;

    // Get the 12-bit Hamming encoded data for the letter
    unsigned short hamming_code = steamed_hams(letter);

    // Distribute the 12 bits into the 4 channels (3 bits per channel)
    for (int i = 3; i >= 0; i--) {
        unsigned int channel_bits = (hamming_code >> (i * 3)) & 0x7;  // Extract 3 bits for each channel
        pixel_bytes |= (channel_bits << (i * 8));  // Insert into the 3 LSBs of each channel
    }

    if (verbose) {
        printf("#%x\n", pixel_bytes);
    }

    return pixel_bytes;
}


void encode_image(unsigned int message_size, unsigned char* img, char* message, bool verbose) {
    for (unsigned int i = 0; i < message_size; i++) {
        unsigned int pixel_bytes = 0;

        // Extract the 4-channel pixel and pack it into an int
        for (char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        }

        // Encode the character into the pixel using Hamming (12,8)
        unsigned int encoded_pixel = encode_pixel(pixel_bytes, message[i], verbose);

        // Place the encoded pixel back into the image
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
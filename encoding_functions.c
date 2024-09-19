#include "encoding_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


unsigned short steamed_hams(char letter) {
    unsigned short encoded = 0;

    unsigned short g1, g2, g3 = 0;

    /* Splits letter into groups */
    g1 = letter & 0b10000000;
    g2 = letter & 0b01110000;
    g3 = letter & 0b00001111;

    g1 <<= 2;
    g2 <<= 1;

    /* Puts the groups into the respective bits for the Hamming code */
    encoded = (g1 | g2 | g3);

    unsigned short p1, p2, p4, p8 = 0;

    /* Hamming(12, 8) layout: P1 P2 D1 P4 D2 D3 D4 P8 D5 D6 D7 D8     */
    /* P1 counts how many bits in poisition with a 1 in the 1's place */
    /* P2 counts how many bits in poisition with a 1 in the 2's place */
    /* P4 counts how many bits in poisition with a 1 in the 4's place */
    /* P8 counts how many bits in poisition with a 1 in the 8's place */

    p1 = ((encoded >> 9) ^ (encoded >> 7) ^ (encoded >> 5) ^ (encoded >> 3) ^ (encoded >> 1)) & 1;
    p2 = ((encoded >> 9) ^ (encoded >> 6) ^ (encoded >> 5) ^ (encoded >> 2) ^ (encoded >> 1)) & 1;
    p4 = ((encoded >> 7) ^ (encoded >> 6) ^ (encoded >> 5)) & 1;
    p8 = ((encoded >> 3) ^ (encoded >> 2) ^ (encoded >> 1)) & 1;

    p1 <<= 11;
    p2 <<= 10;
    p4 <<= 8;
    p8 <<= 4;

    /* Puts the parity bits into the respective bits */
    encoded |= (p1 | p2 | p4 | p8);

    return encoded;
} // steamed_hams

unsigned int encode_pixel(unsigned int pixel_bytes, char letter, bool verbose) {
    if (verbose) {
        printf("Encoding %c into #%x:\t", letter, pixel_bytes);
    } // if

    /* Clear the 3 LSBs for each channel */
    unsigned int bit_mask = 0xF8F8F8F8;

    pixel_bytes &= bit_mask;

    /* Get the 12-bit Hamming encoded data for the letter */
    unsigned short hamming_code = steamed_hams(letter);

    /* Spread the 12 bits into the 4 channels (3 bits per channel) 
       Needs to be unsigned because it counts down and includes 0 (all unsigned 0 - 1 = 255 and will loop forever) */
    for (char i = 3; i >= 0; i--) {
        unsigned int channel_bits = (hamming_code >> (i * 3)) & 0x7;  /* Extract 3 bits for each channel */
        pixel_bytes |= (channel_bits << (i * 8));  /* Insert into the 3 LSBs of each channel */
    } // for

    if (verbose) {
        printf("#%x\n", pixel_bytes);
    } // if

    return pixel_bytes;
} // encode_pixel


void encode_image(unsigned int message_size, unsigned char* img, char* message, bool verbose) {
    for (unsigned int i = 0; i < message_size; i++) {
        unsigned int pixel_bytes = 0;

        /* Extract the 4-channel pixel and pack it into an int */
        for (unsigned char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        } // for

        /* Encode the character into the pixel using Hamming(12,8) */
        unsigned int encoded_pixel = encode_pixel(pixel_bytes, message[i], verbose);

        /* Place the encoded pixel back into the image */
        for (unsigned char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            img[pixel_index] = (encoded_pixel >> (3 - j) * 8);
        } // for
    } // for
} // encode_image

void add_meta(char* message, unsigned char num_meta_bytes, char* meta_bytes) {
    for (unsigned char i = 0; i < num_meta_bytes; i++) {
        message[i] = meta_bytes[i];
    } // for
} // add_meta
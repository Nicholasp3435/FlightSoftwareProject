#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "decoding_functions.h"
#include "common_functions.h"

unsigned char hamming_decode(unsigned short hamming_code) {  
    /* Counts the parity bits of the Hamming code */
    unsigned char p1 = ((hamming_code >> 11) ^ (hamming_code >> 9) ^ (hamming_code >> 7) ^
                        (hamming_code >> 5)  ^ (hamming_code >> 3) ^ (hamming_code >> 1)) & 1;

    unsigned char p2 = ((hamming_code >> 10) ^ (hamming_code >> 9) ^ (hamming_code >> 6) ^
                        (hamming_code >> 5)  ^ (hamming_code >> 2) ^ (hamming_code >> 1)) & 1;

    unsigned char p4 = ((hamming_code >> 8) ^ (hamming_code >> 7) ^ (hamming_code >> 6) ^
                        (hamming_code >> 5) ^ (hamming_code)) & 1;

    unsigned char p8 = ((hamming_code >> 4) ^ (hamming_code >> 3) ^ (hamming_code >> 2) ^ 
                        (hamming_code >> 1) ^ (hamming_code)) & 1;

    p2 <<= 1;
    p4 <<= 2;
    p8 <<= 3;

    unsigned char error_location = (p1 | p2 | p4 | p8);

    /* if error_location != 0, flip the bit at that position */
    if (error_location != 0)  {
        unsigned short bit_mask = 0x1000;
        bit_mask >>= error_location;
        hamming_code ^= bit_mask;
    } // if

    unsigned short g1, g2, g3 = 0;

    g1 = (hamming_code & 0x200) >> 2;
    g2 = (hamming_code & 0xE0) >> 1;
    g3 =  hamming_code & 0xF;

    unsigned char letter = (g1 | g2 | g3);

    return letter;
} // hamming_decode

unsigned char decode_pixel(unsigned int pixel_bytes, bool verbose) {
    if (verbose) {
        printf("Decoding #%x:\t", pixel_bytes);
    } // if

    unsigned short hamming_code = 0;

    /* Extracts the letter data from the pixel */
    for (char i = 3; i >= 0; i--) {
        unsigned int channel_bits = (pixel_bytes >> (i * 8)) & 0x7; /* Extracts 3 bits for each channel*/
        hamming_code |= (channel_bits << (i * 3)); /* Inserts into the hamming code */
    } // for

    unsigned char letter = hamming_decode(hamming_code);

    if (verbose) {
        printf("Decoded %c out.\n", letter);
    } // if

    return letter;
} // decode_pixel

bool decode_image(unsigned char* img, FILE* fptr, bool verbose) {

    /* "Nic" is encoded into every first 3 pixels of each image by encode.c */
    const char* signature = "Nic";

    printf("Checking metadata . . .\n");

    /* Checks the signature */
    for (unsigned char i = 0; i < 3; i++) {

        unsigned int pixel_bytes = extract_pixel_bytes(i, img);

        unsigned char letter = decode_pixel(pixel_bytes, verbose);

        if (letter != signature[i]) {
            printf("Error: Image doesn't seem to be encoded.\n");
            return false;
        } // if
    } // for

    unsigned int message_size = 0;

    /* Gets the length of the message. The length is encoded in pixels [3, 7] */
    for (unsigned char i = 3; i < 7; i++) {

        unsigned int pixel_bytes = extract_pixel_bytes(i, img);

        unsigned int byte = decode_pixel(pixel_bytes, verbose);

        message_size |= (byte << ((6 - i) * 8));
    } // for

    printf("Found %u letters encoded . . .\n", message_size);

    unsigned int num_meta_bytes = 7;

    /* Decodes the rest of the pixels and writes the message to the file */
    for (unsigned int i = num_meta_bytes; i < message_size; i++) {

        unsigned int pixel_bytes = extract_pixel_bytes(i, img);

        unsigned char letter = decode_pixel(pixel_bytes, verbose);

        fprintf(fptr, "%c", letter);
    } // for

    puts("");

    return true;
} // decode_image

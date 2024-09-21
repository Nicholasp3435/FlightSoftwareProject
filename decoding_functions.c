#include <stdio.h>
#include <stdbool.h>
#include <string.h>


unsigned char hamming_decode(unsigned short hamming_code) {  
    /* Count all the parity bits */
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

    /* error_location != 0, flip the bit at that position */
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

    const char* signature = "Nic";

    printf("Checking metadata . . .\n");

    /* Checks the signature to see if it is a valid encoded image */
    for (unsigned char i = 0; i < 3; i++) {
        unsigned int pixel_bytes = 0;

        /* Extract the 4-channel pixel and pack it into an int */
        for (unsigned char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        } // for

        unsigned char letter = decode_pixel(pixel_bytes, verbose);

        if (letter != signature[i]) {
            printf("Error: Image doesn't seem to be encoded.\n");
            return false;
        }
    }

    unsigned int message_size = 0;

    /* Gets the length of the message */
    for (unsigned char i = 0; i < 4; i++) {
        unsigned int pixel_bytes = 0;

        /* Extract the 4-channel pixel and pack it into an int */
        for (unsigned char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j + (3 * 4); /* offset by 3 pixels with 4 channels each */
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        } // for

        unsigned int byte = decode_pixel(pixel_bytes, verbose);

        message_size |= (byte << ((3 - i) * 8));
    }

    printf("Found %u letters encoded . . .\n", message_size);

    unsigned int num_meta_bytes = 7;

    for (unsigned int i = num_meta_bytes; i < message_size; i++) {
        /* Fancy dynamic display adapted from: https://stackoverflow.com/q/20947161 */
        printf("\rDecoded %u of %u letters . . .", i + 1, message_size);
        fflush(stdout);

        unsigned int pixel_bytes = 0;

        /* Extract the 4-channel pixel and pack it into an int */
        for (unsigned char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        } // for

        unsigned char letter = decode_pixel(pixel_bytes, verbose);

        fprintf(fptr, "%c", letter);
    }

    puts("");

    return true;
}

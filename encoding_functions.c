#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "encoding_functions.h"
#include "common_functions.h"

unsigned short steamed_hams(char letter) {
    unsigned short encoded = 0;

    unsigned short g1, g2, g3 = 0;

    /* Splits letter into groups */
    g1 = letter & 0x80;
    g2 = letter & 0x70;
    g3 = letter & 0xF;

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
    p4 = ((encoded >> 7) ^ (encoded >> 6) ^ (encoded >> 5) ^ (encoded)) & 1;
    p8 = ((encoded >> 3) ^ (encoded >> 2) ^ (encoded >> 1) ^ (encoded)) & 1;

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

    /* Spread the 12 bits into the 4 channels (3 bits per channel) */
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

        unsigned int pixel_bytes = extract_pixel_bytes(i, img);

        /* Encode the character into the pixel using Hamming(12,8) */
        unsigned int encoded_pixel = encode_pixel(pixel_bytes, message[i], verbose);

        /* Place the encoded pixel back into the image */
        for (unsigned char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * i + j;
            img[pixel_index] = (encoded_pixel >> (3 - j) * 8);
        } // for
    } // for
    puts("");

} // encode_image

char* read_message_from_file(const char* mesasage_txt_name, unsigned int total_pixels,
                             unsigned char meta_size, unsigned int* message_size_ptr) {

    /* File reading adapted from: https://www.w3schools.com/c/c_files_read.php */
    FILE *fptr;
    unsigned int message_size;
    
    /* Opens a file into read mode */
    fptr = fopen(mesasage_txt_name, "r");
    if (fptr == NULL) {
        printf("Error: Failed to open message file %s\n", mesasage_txt_name);
        return NULL;
    } // if

    /* Gets the length of the file */
    fseek(fptr, 0L, SEEK_END);
    message_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    message_size += meta_size;

    /* Checks if message_size is too much for image encoding. If so, truncate the message. */
    if (message_size > total_pixels) {
        printf("\tWarning: message is too big to be encoded into this image; truncating message.\n");
        message_size = total_pixels;
    } // if

    /* Allocates memory for the message (using calloc to initialize memory to 0) */
    char* message = (char*) calloc(message_size, 1);
    if (message == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(fptr);
        return NULL;
    } // if

    /* Add metadata to the message */
    char meta_bytes[meta_size];
    printf("Adding meta bytes . . . \n");

    add_meta(meta_bytes, message, meta_size, message_size);

    /* Reads the file 1 byte at a time up to the message_size without the metadata
       and puts the data offset by the metadata into message */
    fread(message + meta_size, 1, message_size - meta_size, fptr);

    /* Close file */
    fclose(fptr);

    /* Null-terminate the message */
    message[message_size - 1] = '\0';

    *message_size_ptr = message_size;

    /* Return a pointer to the allocated memory */
    return message;
} // read_message_from_file


void add_meta(char* meta_bytes, char* message, unsigned char meta_size, unsigned int message_size) {
    /* Puts a signature into the metadate so the decoder to know the image was or wasn't encoded */
    meta_bytes[0] = 'N';
    meta_bytes[1] = 'i';
    meta_bytes[2] = 'c';

    /* Puts the message_size into metadata (int = 4 chars) */
    for (unsigned char i = 0; i < 4; i++) {
        meta_bytes[(3 - i) + 3] = ((message_size) >> (i * 8));

    } // for

    for (unsigned char i = 0; i < meta_size; i++) {
        message[i] = meta_bytes[i];
    } // for
} // add_meta

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image.h"

#include "encoding_functions.h"
#include "decoding_functions.h"
#include "common_functions.h"

/* Fancy colors from https://stackoverflow.com/a/23657072 */
#define RED   "\x1B[31m \tFAILED: "
#define GRN   "\x1B[32m \tPASSED: "
#define RESET "\x1B[0m"

/**
 * Function: test_steamed_hams
 * ---------------------------
 *   Runs a test on the steamed_hams function.
 * 
 *   letter: The letter to test the steamed_hams function with.
 *   expected_result: The expected output of steamed_hams with letter as it's input.
 * 
 *   returns: 0 if the test was successful; 1 otherwise.
 */
char test_steamed_hams(unsigned char letter, unsigned short expected_result) {
    unsigned short result = steamed_hams(letter);

    if (result == expected_result) {
        printf(GRN "steamed_hams('%c')\n" RESET, letter);
        return 0;
    } else {
        printf(RED "steamed_hams('%c') Expected: %x, Got: %x\n" RESET, letter, expected_result, result);
        return 1;
    } // if
} // test_steamed_hams

/**
 * Function: test_hamming_decode
 * ---------------------------
 *   Runs a test on the hamming_decode function.
 * 
 *   hamming_code: The hamming code to test the hamming_decode function with.
 *   expected_result: The expected output of hamming_decode with hamming_code as it's input.
 * 
 *   returns: 0 if the test was successful; 1 otherwise.
 */
char test_hamming_decode(unsigned short hamming_code, unsigned char expected_result) {
    unsigned char result = hamming_decode(hamming_code);

    if (result == expected_result) {
        printf(GRN "hamming_decode(0x%x)\n" RESET, hamming_code);
        return 0;
    } else {
        printf(RED "hamming_decode(0x%x) Expected: %c, Got: %c\n" RESET, hamming_code, expected_result, result);
        return 1;
    } // if
} // test_hamming_decode

/**
 * Function: test_extract_pixel_bytes
 * ---------------------------
 *   Runs a test on the extract_pixel_bytes function on the "test.png" image.
 * 
 *   index: The index to test the extract_pixel_bytes function with.
 *   expected_result: The expected output of extract_pixel_bytes with index as it's input.
 * 
 *   returns: 0 if the test was successful; 1 otherwise.
 */
char test_extract_pixel_bytes(unsigned int index, unsigned int expected_result) {
    int width, height, channels; 
    const unsigned char num_channels = 4; 
    
    unsigned char *img = stbi_load("test.png", &width, &height, &channels, num_channels);
    if (img == NULL) {
        printf("Error: Failed to load image\n");
        return 1;
    } // if

    unsigned int result = extract_pixel_bytes(index, img);

    stbi_image_free(img); 

    if (result == expected_result) {
        printf(GRN "extract_pixel_bytes(%u, img)\n" RESET, index);
        return 0;
    } else {
        printf(RED "extract_pixel_bytes(%u, img) Expected: %x, Got: %x\n" RESET, index, expected_result, result);
        return 1;
    } // if
}

/**
 * Function: test_encode_pixel
 * ---------------------------
 *   Runs a test on the encoded_pixel function.
 * 
 *   pixel_bytes: The color data of a pixel, i.e. it's hex value.
 *   letter: The letter to encode into the pixel.
 *   expected_result: The expected output of encode_pixel with pixel_bytes and letter as it's inputs.
 * 
 *   returns: 0 if the test was successful; 1 otherwise.
 */
char test_encode_pixel(unsigned int pixel_bytes, unsigned char letter, unsigned int expected_result) {
    unsigned int encoded_pixel = encode_pixel(pixel_bytes, letter, false);

    if (encoded_pixel == expected_result) {
        printf(GRN "encode_pixel(0x%x, '%c')\n" RESET, pixel_bytes, letter);
        return 0;
    } else {
        printf(RED "encode_pixel(0x%x, '%c') Expected: 0x%x, Got: 0x%x\n" RESET, 
               pixel_bytes, letter, expected_result, encoded_pixel);
        return 1;
    } // if
} // test_encode_pixel

/**
 * Function: test_decode_pixel
 * ---------------------------
 *   Runs a test on the decode_pixel function.
 * 
 *   pixel_bytes: The color data of a pixel, i.e. it's hex value.
 *   expected_result: The expected output of decode_pixel with pixel_bytes as it's input.
 *      This is the letter encoded in the pixel.
 * 
 *   returns: 0 if the test was successful; 1 otherwise.
 */
char test_decode_pixel(unsigned int pixel_bytes, unsigned char expected_result) {
    unsigned char decoded_letter = decode_pixel(pixel_bytes, false);

    if (decoded_letter == expected_result) {
        printf(GRN "decode_pixel(0x%x)\n" RESET, pixel_bytes);
        return 0;
    } else {
        printf(RED "decode_pixel(0x%x) Expected: %c, Got: %c\n" RESET,
               pixel_bytes, expected_result, decoded_letter);
        return 1;
    } // if
} // test_decode_pixel

/**
 * Function: main
 * --------------
 *   This runs a series of unit tests on the encoding and decoding functions.
 * 
 *   returns: The number of tests that failed.
 */
int main() {
    unsigned char errors = 0;

    printf("Running tests...\n");

    printf("Testing steamed_hams function...\n");
    errors += test_steamed_hams('A', 0x891);
    errors += test_steamed_hams('B', 0x592);
    errors += test_steamed_hams('C', 0x483);

    printf("Testing hamming_decode function...\n");
    /* Tests without bit errors */
    errors += test_hamming_decode(0x891, 'A');
    errors += test_hamming_decode(0x592, 'B');
    errors += test_hamming_decode(0x483, 'C');
    
    /* Tests bit errors */
    errors += test_hamming_decode(0x890, 'A');
    errors += test_hamming_decode(0x5B2, 'B');
    errors += test_hamming_decode(0x4C3, 'C');

    printf("Testing extract_pixel_bytes . . .\n");
    errors += test_extract_pixel_bytes(0, 0xD70071FF);
    errors += test_extract_pixel_bytes(1, 0x9C4E97FF);
    errors += test_extract_pixel_bytes(2, 0x0035AAFF);

    printf("Testing encode_pixel function . . .\n");
    errors += test_encode_pixel(0xD70071FF, 'A', 0xD40272F9);
    errors += test_encode_pixel(0x9C4E97FF, 'B', 0x9A4E92FA);
    errors += test_encode_pixel(0x0035AAFF, 'C', 0x0232A8FB);

    printf("Testing decode_pixel function...\n");
    /* Tests without bit errors */
    errors += test_decode_pixel(0xD40272F9, 'A');
    errors += test_decode_pixel(0x9A4E92FA, 'B');
    errors += test_decode_pixel(0x0232A8FB, 'C');

    /* Tests bit errors */
    errors += test_decode_pixel(0xD40273F9, 'A');
    errors += test_decode_pixel(0x9A4E92FE, 'B');
    errors += test_decode_pixel(0x0032A8FB, 'C');

    printf("All tests finished.\n");

    return errors;
} // main

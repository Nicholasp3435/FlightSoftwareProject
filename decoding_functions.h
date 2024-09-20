#ifndef ENCODING_FUNCTIONS_H
#define ENCODING_FUNCTIONS_H

#include <stdbool.h>

/**
 * Function: hamming_decode
 * ------------------------
 *   Checks a Hamming(12, 8) code to make sure that the data is intact.
 *   This method can check and fix 1 bit errors. 
 * 
 *   hamming_code: the Hamming code to check
 * 
 *   returns: the decoded letter
 */
unsigned char hamming_decode(unsigned short hamming_code);

/**
 * Function: decode_pixel
 * ----------------------
 *   Decodes the the letter out of a pixel. Pixels are expected to have 4 channels: R, G, B, and A.
 *   This automatically decodes the Hamming code from the data of the pixel. It will take the last
 *   3 bits of each channel and put them all together, decode the Hamming code, and return the letter.
 * 
 *   pixel_bytes: 32-bit integer with each byte for each of the channels in the order RGBA
 *   verbose: If true, the function will print the letter after decoding
 * 
 *   returns: a char of the letter encoded in the pixel
 */
unsigned char decode_pixel(unsigned int pixel_pytes, bool verbose);

/**
 * Function: decode_image
 * ----------------------
 *   Decodes all of the pixels of an image. This uses the decode_pixel function.
 * 
 *   img: An array (provided by stb) of all the subpixels of an image
 *   output_txt: The filename to write the output message to
 *   verbose: If true, the function will print the letter after decoding
 * 
 *   returns: True if the decoding was successful
 */
bool decode_image(unsigned char* img, FILE* fptr, bool verbose);

#endif
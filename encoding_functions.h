#ifndef ENCODING_FUNCTIONS_H
#define ENCODING_FUNCTIONS_H

#include <stdbool.h>

/* 
 * Function: steamed_hams
 * ----------------------
 *   Encodes 4 parity bits for a Hamming(12, 8) code, i.e. it encodes a byte of data into a Hamming code.
 *   Hamming codes allow for self correcting data in case of transmission errors.
 *   "D'oh no, I said Steamed Hams! That's what I call Hamming codes!"
 * 
 *   data: The byte to encode with a Hamming code. 
 *   
 *   returns: A short with it's 12 LSBs populated by data and parity bits as follows: 
 *      [0 0 0 0 P1 P2 D1 P4 D2 D3 D4 P8 D5 D6 D7 D8]
 */
unsigned short steamed_hams(char data);

/*
 * Function encode_pixel
 * ---------------------
 *   Encodes a letter into a pixel. Pixels are expected to have 4 channels: R, G, B, and A.
 *   This automatically adds Hamming codes to the letter. It takes the 12 bits and spreads
 *   them among the 4 channels. Each byte of the channel gets it's last 3 LSBs used for data.
 *   Observe this diagram of each bit:
 *   
 *      R is a red bit
 *      B is a blue bit
 *      G is a green bit
 *      A is an alpha bit
 *      D is a data bit
 *      
 *      RRRRRRRR | GGGGGGGG | BBBBBBBB | AAAAAAAA -> encode_pixel -> RRRRRDDD | GGGGGDDD | BBBBBDDD | AAAAADDD
 *   
 *   The resulting pixel is similar enough to the original as to hide the data without too much suspicion.
 * 
 *   pixel_bytes: The color data of a pixel, i.e. it's hex value
 *   letter: The letter to store inside the bit
 *   verbose: If true, the function will print the hex value after the encoding
 * 
 *   returns: A 32-bit integer with the encoded data, i.e. it's hex value with the letter encoded
 */
unsigned int encode_pixel(unsigned int pixel_bytes, char letter, bool verbose);

/*
 * Function: encode_image
 * ----------------------
 *   Encodes all the pixels of an image. It loops through all the pixels using the encode_pixel function.
 * 
 *   message_size: The amount of characters to put into the image
 *   img: An array (provided by stb) of all the subpixels of an image
 *   message: The message to put into the image
 *   verbose: If true, the function will print the hex value after the encoding
 */
void encode_image(unsigned int message_size, unsigned char* img, char* message, bool verbose);

/*
 * Function read_message_from_file
 * -------------------------------
 *   Reads a message from a file. It will make sure that the message size isn't too much for the
 *   image to handle. The message will contain metadata for the length of the message and a signature
 *   of "Nic". The lack of one indicates that the image doesn't have anything encoded.
 * 
 *   mesasage_txt_name: The name of the message_txt file to read
 *   total_pixels: The total nuber of pixels of the image.
 *      Used to check if all the message will or won't fit.
 *   meta_size: The number of bytes to allocate for the metadata
 *   message_size: A pointer to place the final size of the message in
 * 
 *   returns: a char* of the message out of the file
 */
char* read_message_from_file(const char* mesasage_txt_name, unsigned int total_pixels,
                             unsigned char meta_size, unsigned int* message_size);

/*
 * Function: add_meta
 * ------------------
 *   Adds the metadata to the begnning of the message. Use this before reading the 
 *   message file to prepend with the metadata.
 *   
 *   meta_bytes: The array of metadata to copy to the message
 *   message: The array to put the metadata in
 *   meta_size: The number of metadata bytes
 *   message_size: The size of the message to put as meta
 */
void add_meta(char* meta_bytes, char* message, unsigned char meta_size, unsigned int message_size);

#endif
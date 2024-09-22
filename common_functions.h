#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

/**
 * Function: extract_pixel_bytes
 * ----------------------------
 *   A function that can extract the color data of a specified pixel of an image.
 * 
 *   index: The index of the pixel to get.
 *   img: The image to extarct the data from
 * 
 *   returns: A 32-bit integer of the pixel color data, i.e. it's hex value.
 */
unsigned int extract_pixel_bytes(unsigned int index, unsigned char* img);

#endif

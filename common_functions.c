#include "common_functions.h"

unsigned int extract_pixel_bytes(unsigned int index, unsigned char* img) {
        unsigned int pixel_bytes = 0;

        /* Extract the 4-channel pixel and pack it into an int */
        for (unsigned char j = 0; j < 4; j++) {
            unsigned int pixel_index = 4 * index + j;
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        } // for

        return pixel_bytes;
} // extract_pixel_bytes
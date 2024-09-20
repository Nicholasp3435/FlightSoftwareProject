#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Enable stb implementations */
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

/** https://github.com/nothings/stb/blob/master/stb_image.h */
#include "include/stb_image.h"
/** https://github.com/nothings/stb/blob/master/stb_image_write.h */
#include "include/stb_image_write.h"

#include "encoding_functions.h"

/*
 * Function: main
 * --------------
 *   The main function encodesa message into a PNG image using Hamming(12,8) code for error correction.
 *   It processes command line arguments for input, output, and message file paths, loads the image,
 *   reads the message, and then encodes the messageinto the image pixels. Finally, the modified image
 *   is saved as a new or overwrites a PNG file.
 *
 *   The steps of the program are as follows:
 *     1. Parse command line arguments to get the input/output file names.
 *     2. Load the input PNG image using stb_image.
 *     3. Open and read the message from the specified text file.
 *     4. Add metadata to the message (signature & message size).
 *     5. Encode the message into the image pixels using Hamming code.
 *     6. Save the modified image to a new PNG file.
 *
 *   argc: The number of command line arguments.
 *   argv: An array of strings representing the command line arguments.
 *
 *   returns: EXIT_SUCCESS (0) if the encoding worked successfully; else, EXIT_FAILURE (1).
 */
int main(int argc, char * argv[]) {
    // Default files
    char* input_png_name = "image.png";
    char* output_png_name = "output.png";
    char* mesasage_txt_name = "message.txt";

    // Parses the command line args
    for (unsigned char i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            input_png_name = argv[i + 1];
        } else if (strcmp(argv[i], "-o") == 0) {
            output_png_name = argv[i + 1];
        } else if (strcmp(argv[i], "-m") == 0) {
            mesasage_txt_name = argv[i + 1];
        } // if
    } // for

    /* Initializes the width, height, and channels of the image for stbi_load */
    int width, height, channels; 
    
    /* Used to always set there 4 subpixels per pixel */
    unsigned char num_channels = 4; 
    
    /* Load the PNG image and checks if it was successful. Exits if it didn't */
    unsigned char *img = stbi_load(input_png_name, &width, &height, &channels, num_channels);
    if (img == NULL) {
        printf("Error: Failed to load image\n");
        return EXIT_FAILURE;
    } // if

    unsigned int total_pixels = width * height;

    printf("Loaded %s with width %d and height %d; %d pixels total\n", 
        input_png_name, width, height, total_pixels);

    printf("\t(That means we can encode %d ascii characters in it!)\n\n", total_pixels);

    printf("Reading %s for message . . .\n", mesasage_txt_name);

    /* File reading adapted from: https://www.w3schools.com/c/c_files_read.php */
    FILE *fptr;

    /* Opens a file into read mode */
    fptr = fopen(mesasage_txt_name, "r"); 

    /* Gets the length of the file */
    fseek(fptr, 0L, SEEK_END);
    unsigned int message_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    /* Sets the amout of bytes to allocate to the message for metadata */
    unsigned char num_meta_bytes = 7;
    message_size += num_meta_bytes;

    /* Checks if message_size is too much for image encoding. If so, truncate the message. */
    if (message_size > (total_pixels)) {
        printf("\tWarning: message is too big to be encoded into this image; truncating message.\n");
        message_size = total_pixels;
    } // if

    char message[message_size];
    char meta_bytes[num_meta_bytes];

    printf("Adding meta bytes . . . \n");

    /* Puts a signature into the metadate so the decoder to know the image was or wasn't encoded */
    meta_bytes[0] = 'N';
    meta_bytes[1] = 'i';
    meta_bytes[2] = 'c';

    /* Puts the message_size into metadata (int = 4 chars) */
    for (unsigned char i = 0; i < 4; i++) {
        meta_bytes[(3 - i) + 3] = ((message_size) >> (i * 8));
    } // for

    add_meta(message, num_meta_bytes, meta_bytes);
    
    /* Reads the file 1 byte at a time up to the message_size without the metadata
       and puts the data offset by the metadata into message */
    fread((message + num_meta_bytes), 1, message_size - num_meta_bytes, fptr); 

    message[message_size - 1] = '\0'; 

    /* Close file */
    fclose(fptr); 

    printf("Encoding %d characters into pixels . . .\n", message_size);

    encode_image(message_size, img, message, false); /* WOOO encoding finally! */

    printf("Finished encoding %u characters into the pixels!\n\n", message_size);
    printf("Writing to %s . . .\n", output_png_name);

    /* Write the modified image back to a PNG file.
       It seems that this is what is causing any slowdown. Perhaps find a quicker header to do this? */
    if (!stbi_write_png(output_png_name, width, height, num_channels, img, width * num_channels)) {
        printf("Error: Failed to save image\n");
        return 1;
    } // if

    /* Free the image from memory */
    stbi_image_free(img); 

    printf("Image processing complete\n");
    return EXIT_SUCCESS;
} // main
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Enable stb implementations
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image.h"
#include "include/stb_image_write.h"

#include "decoding_functions.h"

int main(int argc, char * argv[]) {
    // Default files
    char* input_png_name = "output.png";
    char* output_txt_name = "message_output.txt";

    // Parses the command line args
    for (unsigned char i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            input_png_name = argv[i + 1];
        } else if (strcmp(argv[i], "-o") == 0) {
            output_txt_name = argv[i + 1];
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

    printf("Loaded %s with width %d and height %d; %d pixels total\n\n", 
        input_png_name, width, height, total_pixels);


    FILE *fptr;

    /* Open a file in writing mode */
    fptr = fopen(output_txt_name, "w");

    printf("Decoding image . . .\n");
    bool success = decode_image(img, fptr, false);
    printf("Finished decoding pixels! Wrote message to %s\n", output_txt_name);

    // Close the file
    fclose(fptr); 

    stbi_image_free(img); 

    if (!success) {
        return EXIT_FAILURE;
    }

}
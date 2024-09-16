#include <stdio.h>
#include <stdlib.h>

// Enable stb implementations
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image.h"
#include "include/stb_image_write.h"

int main(int argc, char * argv[]) {
    // checks the command line arguments to make sure everything is formatted properly

    // makes sure there isn't too many or too little args
    if (argc < 4 || argc > 5) {
        printf("Incorrect command format. Refer to README.md for uasage\n");
        return EXIT_FAILURE; 
    }
    
    char* input_png_name = argv[1];
    char* output_png_name = argv[2];
    char* mesasage_txt_name = argv[3];
    
    // sets the default compression level to 1 bpp
    unsigned int compression_lvl = 1;
    if (argc == 5) {
        // changes the compression level to 1 or 2 depending on the arg
        // also makes sure it is a 1 or 2 and not anything else
        compression_lvl = atoi(argv[4]);
        if (compression_lvl != 1 && compression_lvl != 2) {
            printf("Incorrect command format. Refer to README.md for uasage\n");
            return EXIT_FAILURE; 
        }
    }

    int width, height, channels;
    int num_channels = 4;
    
    // Load the PNG image
    unsigned char *img = stbi_load(input_png_name, &width, &height, &channels, num_channels);
    if (img == NULL) {
        printf("Failed to load image\n");
        return EXIT_FAILURE;
    }

    printf("Loaded %s with width %d and height %d; %d pixels total\n", input_png_name, width, height, width * height);
    unsigned int total_bytes = width * height * compression_lvl;
    printf("(That means we can encode %d Kb of ascii characters in it!)\n\n", width * height * compression_lvl / 1024);

    printf("Reading %s for message. . .\n",mesasage_txt_name);

    FILE *fptr;

    // Open a file in read mode
    fptr = fopen(mesasage_txt_name, "r"); 

    char message[total_bytes + 1];
    fread(message, 1, total_bytes, fptr);
    message[total_bytes] = '\0';

    // Close the file
    fclose(fptr); 

    printf("%s", message);

    for (unsigned int i = 0; i < width * height * num_channels; i++) {
        img[i] = img[i] & 0b11110000;
    }

    // Write the modified image back to a PNG file
    if (!stbi_write_png(output_png_name, width, height, num_channels, img, width * num_channels)) {
        printf("Failed to save image\n");
        return 1;
    }

    // Free the image memory
    stbi_image_free(img);

    printf("Image processing complete\n");
    return EXIT_SUCCESS;
}
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
            printf("Error: Incorrect command format. Refer to README.md for uasage\n");
            return EXIT_FAILURE; 
        }
    }

    int width, height, channels;
    int num_channels = 4;
    
    // Load the PNG image
    char *img = stbi_load(input_png_name, &width, &height, &channels, num_channels);
    if (img == NULL) {
        printf("Error: Failed to load image\n");
        return EXIT_FAILURE;
    }
    unsigned int total_pixels = width * height;

    printf("Loaded %s with width %d and height %d; %d pixels total\n", input_png_name, width, height, total_pixels);
    
    printf("(That means we can encode %d ascii characters in it!)\n\n", total_pixels * compression_lvl);

    printf("Reading %s for message. . .\n", mesasage_txt_name);

    FILE *fptr;

    // Open a file in read mode
    fptr = fopen(mesasage_txt_name, "r"); 

    char message[total_pixels];
    fread(message, 1, total_pixels, fptr);
    message[total_pixels - 1] = '\0';

    // Close the file
    fclose(fptr); 

    unsigned int message_length = strlen(message);

    printf("Encoding %d characters\n\n", message_length);

    printf("Encoding characters into image ...\n");


    //TODO: make this not look terrible like i mean it works butttttt likeeee wtf
    for (int i = 0; i < message_length; i++) {
        unsigned char img_r_byte = img[4 * i + 0] & 0b11111100;
        unsigned char img_g_byte = img[4 * i + 1] & 0b11111100;
        unsigned char img_b_byte = img[4 * i + 2] & 0b11111100;
        unsigned char img_a_byte = img[4 * i + 3] & 0b11111100;

        unsigned char message_byte = message[i];

        unsigned char message_r_bits = (message_byte & 0b11000000) >> 6;
        unsigned char message_g_bits = (message_byte & 0b00110000) >> 4;
        unsigned char message_b_bits = (message_byte & 0b00001100) >> 2;
        unsigned char message_a_bits = (message_byte & 0b00000011) >> 0;

        img_r_byte |= message_r_bits;
        img_g_byte |= message_g_bits;
        img_b_byte |= message_b_bits;
        img_a_byte |= message_a_bits;

        img[4 * i + 0] = img_r_byte;
        img[4 * i + 1] = img_g_byte;
        img[4 * i + 2] = img_b_byte;
        img[4 * i + 3] = img_a_byte;        
    }

    // debugging
    for (int i = 0; i < total_pixels; i++) {
        if (i%4==0) {
            //puts("");
        }
        //printf("%hhu ", img[i]);
    }

    // Write the modified image back to a PNG file
    if (!stbi_write_png(output_png_name, width, height, num_channels, img, width * num_channels)) {
        printf("Error: Failed to save image\n");
        return 1;
    }

    // Free the image memory
    stbi_image_free(img);

    printf("Image processing complete\n");
    return EXIT_SUCCESS;
}
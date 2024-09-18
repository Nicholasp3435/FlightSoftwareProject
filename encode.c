#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Enable stb implementations
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image.h"
#include "include/stb_image_write.h"
#include "encoding_functions.h"

void print_image(unsigned char* img, unsigned int total_sub_pixels) {
    printf("All image pixels (decimal):");
    for (unsigned int i = 0; i < total_sub_pixels; i++) {
        if (i%4==0) {
            puts("");
        }
        printf("%hhu ", img[i]);
    }
    puts("");
}

int main(int argc, char * argv[]) {
    // makes sure there isn't too many or too little args
    if (argc < 4 || argc > 5) {
        printf("Incorrect command format. Refer to README.md for uasage\n");
        return EXIT_FAILURE; 
    }
    
    char* input_png_name = argv[1];
    char* output_png_name = argv[2];
    char* mesasage_txt_name = argv[3];

    int width, height, channels;
    unsigned char num_channels = 4;
    
    // Load the PNG image
    unsigned char *img = stbi_load(input_png_name, &width, &height, &channels, num_channels);
    if (img == NULL) {
        printf("Error: Failed to load image\n");
        return EXIT_FAILURE;
    }
    unsigned int total_pixels = width * height;

    printf("Loaded %s with width %d and height %d; %d pixels total\n", input_png_name, width, height, total_pixels);
    printf("\t(That means we can encode %d ascii characters in it!)\n\n", total_pixels);

    printf("Reading %s for message . . .\n", mesasage_txt_name);

    FILE *fptr;

    // Open the message file in read mode
    fptr = fopen(mesasage_txt_name, "r"); 

    // get's the length of the message
    fseek(fptr, 0L, SEEK_END);
    unsigned int message_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    unsigned char num_meta_bytes = 5;

    message_size = message_size + num_meta_bytes;

        // checks if message_size is too much for image encoding
    if (message_size > (total_pixels)) {
        printf("\tWarning: message is too big to be encoded into this image; truncating message.\n");
        message_size = total_pixels;
    }

    char message[message_size];

    char meta_bytes[num_meta_bytes];

    printf("Adding meta bytes . . . \n");

    // puts the size into meta
    for (unsigned char i = 0; i < 4; i++) {
        meta_bytes[3 - i] = (message_size >> (i * 8));
    }


    add_meta(message, num_meta_bytes, meta_bytes);
    
    fread((message + num_meta_bytes), 1, message_size - num_meta_bytes, fptr);

    message[message_size - 1] = '\0';

    // Close the file
    fclose(fptr); 

    printf("Encoding %d characters into pixels . . .\n", message_size);

    // WOOOOOO ENCODINGGGG !!!!
    encode_image(message_size, img, message, true);


    printf("Finished encoding %u characters into the pixels!\n\n", message_size);
    printf("writing to %s . . .\n", output_png_name);

    // debugging
    print_image(img, total_pixels * 4);


    // Write the modified image back to a PNG file
    // it seems that this is what is causing any slowdown. perhaps find a quicker header to do this?
    if (!stbi_write_png(output_png_name, width, height, num_channels, img, width * num_channels)) {
        printf("Error: Failed to save image\n");
        return 1;
    }

    // Free the image memory
    stbi_image_free(img);

    printf("Image processing complete\n");
    return EXIT_SUCCESS;
}
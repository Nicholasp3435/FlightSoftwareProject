#include <stdio.h>
#include <stdlib.h>

// Enable stb implementations
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image.h"
#include "include/stb_image_write.h"

unsigned int encode_pixel(unsigned int pixel_bytes, char letter, unsigned char compression_lvl) {
    printf("encoding %c into %x\n", letter, pixel_bytes);

    unsigned int encoded_pixel = 0;

    unsigned int bit_mask = 0xFCFCFCFC;
    if (compression_lvl == 2) {
        bit_mask = 0xF0F0F0F0;
    }
    // bit masks for each channel
    pixel_bytes &= bit_mask;

    unsigned char letter_mask = 0b11000000;
    unsigned int split_letter = 0;

    // splits the letter into 4
    for (int i = 3; i >= 0; i--) {
        unsigned int current_channel = letter_mask & letter;
        current_channel = current_channel << (6 * i);
        split_letter |= current_channel;
        letter_mask >>= 2;
    }

    encoded_pixel = pixel_bytes | split_letter;
    printf("%x \n", encoded_pixel);
    return encoded_pixel;
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
    
    // sets the default compression level to 1 bpp
    unsigned char compression_lvl = 1;
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
    unsigned char num_channels = 4;
    
    // Load the PNG image
    unsigned char *img = stbi_load(input_png_name, &width, &height, &channels, num_channels);
    if (img == NULL) {
        printf("Error: Failed to load image\n");
        return EXIT_FAILURE;
    }
    unsigned int total_pixels = width * height;
    unsigned int total_sub_pixels = total_pixels * num_channels;

    printf("Loaded %s with width %d and height %d; %d pixels total\n", input_png_name, width, height, total_pixels);
    
    printf("(That means we can encode %d ascii characters in it!)\n\n", total_pixels * compression_lvl);

    printf("Reading %s for message. . .\n", mesasage_txt_name);

    FILE *fptr;

    // Open the message file in read mode
    fptr = fopen(mesasage_txt_name, "r"); 

    // get's the length of the message
    fseek(fptr, 0L, SEEK_END);
    unsigned int message_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    // checks if message_size is too much for image encoding
    if (message_size > (total_pixels * compression_lvl)) {
        printf("Warning: too big message or too smol image. Truncating message.\n");
        message_size = total_pixels * compression_lvl;
    }

    char message[message_size];
    
    fread(message, 1, message_size, fptr);

    // Close the file
    fclose(fptr); 



    // terminates with null char    
    message[message_size - 1] = '\0';

    printf("Encoding %d characters\n\n", message_size);

    printf("Encoding characters into image ...\n");

    //TODO: make this not look terrible like i mean it works butttttt likeeee wtf
    for (int i = 0; i < message_size; i++) {
        unsigned int pixel_bytes = 0;

        // puts the channels of the pixels into an int for encoding
        for (int j = 0; j < 4; j++) {
            unsigned int channel_byte = img[4 * i + j];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        }    

        unsigned int encoded_pixel = encode_pixel(pixel_bytes, message[i], compression_lvl);

        // places the encoded pixel in the image
        for (int j = 0; j < 4; j++) {
            img[4 * i + j] = (encoded_pixel >> (3-j)*8);
        }
    }

    // debugging
    for (int i = 0; i < total_sub_pixels; i++) {
        if (i%4==0) {
            puts("");
        }
        printf("%hhu ", img[i]);
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
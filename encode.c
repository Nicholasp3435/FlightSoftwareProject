#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Enable stb implementations
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image.h"
#include "include/stb_image_write.h"

unsigned int encode_pixel(unsigned int pixel_bytes, char letter1, char letter2, int compression_level, bool verbose) {
    if (verbose) {
        if (compression_level == 1) {
            printf("Encoding %c into #%x:\t", letter1, pixel_bytes);
        } else {
            printf("Encoding %c and %c into #%x:\t", letter1, letter2, pixel_bytes);
        }
    }

    unsigned int encoded_pixel = 0;
    unsigned int bit_mask = 0xFCFCFCFC;
    if (compression_level == 2) {
        bit_mask = 0xF0F0F0F0;
    }

    // bit masks for each channel
    pixel_bytes &= bit_mask;

    // encoding letter1
    unsigned char letter_mask = 0b11000000;
    unsigned int split_letter1 = 0;

    for (int i = 3; i >= 0; i--) {
        unsigned int current_channel = letter_mask & letter1;
        current_channel = current_channel << (6 * i);
        split_letter1 |= current_channel;
        letter_mask >>= 2;
    }

    if (compression_level == 2) {
        // encoding for letter2 when compression_level is 2
        letter_mask = 0b11000000;
        unsigned int split_letter2 = 0;

        for (int i = 3; i >= 0; i--) {
            unsigned int current_channel = letter_mask & letter2;
            current_channel = current_channel << (6 * i + 2);
            split_letter2 |= current_channel;
            letter_mask >>= 2;
        }

        encoded_pixel = pixel_bytes | (split_letter1 | split_letter2);
    } else {
        // if compression_level == 1, only encode letter1
        encoded_pixel = pixel_bytes | split_letter1;
    }

    if (verbose) {
        printf("#%x\n", encoded_pixel);
    }

    return encoded_pixel;
}

void encode_image(unsigned int message_size, unsigned char* img, char* message, int compression_level, bool verbose) {
    for (unsigned int i = 0; i < message_size; i += compression_level) {
        unsigned int pixel_bytes = 0;

        // puts the channels of the pixels into an int for encoding
        for (char j = 0; j < 4; j++) {
            unsigned int pixel_index = (4 / compression_level) * i + j;
            unsigned int channel_byte = img[pixel_index];
            channel_byte <<= (3 - j) * 8;
            pixel_bytes |= channel_byte;
        }

        // encode based on compression level
        unsigned int encoded_pixel;
        if (compression_level == 1) {
            encoded_pixel = encode_pixel(pixel_bytes, message[i], 0, compression_level, verbose);
        } else {
            encoded_pixel = encode_pixel(pixel_bytes, message[i], message[i + 1], compression_level, verbose);
        }

        // places the encoded pixel in the image
        for (char j = 0; j < 4; j++) {
            unsigned int pixel_index = (4 / compression_level) * i + j;
            img[pixel_index] = (encoded_pixel >> (3 - j) * 8);
        }
    }
}


void add_meta(char* message, unsigned char num_meta_bytes, char* meta_bytes) {
    for (unsigned char i = 0; i < num_meta_bytes; i++) {
        message[i] = meta_bytes[i];
    }
}

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

    printf("Loaded %s with width %d and height %d; %d pixels total\n", input_png_name, width, height, total_pixels);
    printf("\t(That means we can encode %d ascii characters in it!)\n\n", total_pixels * compression_lvl);

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
    if (message_size > (total_pixels * compression_lvl)) {
        printf("\tWarning: message is too big to be encoded into this image; truncating message.\n");
        if (compression_lvl == 1) {
            printf("\t(Try using a higher compression level!).\n");
        }
        puts("");
        message_size = total_pixels * compression_lvl;
    }

    char message[message_size];

    char meta_bytes[num_meta_bytes];

    printf("Adding meta bytes . . . \n");
    // puts the size into meta
    for (unsigned char i = 0; i < 4; i++) {
        meta_bytes[i] = '\0';
    }
    add_meta(message, num_meta_bytes, meta_bytes);
    
    fread((message + num_meta_bytes), 1, message_size - num_meta_bytes, fptr);

    message[message_size - 1] = '\0';

    // Close the file
    fclose(fptr); 

    printf("Encoding %d characters into pixels . . .\n", message_size);

    // WOOOOOO ENCODINGGGG !!!!
    encode_image(message_size, img, message, compression_lvl, true);


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
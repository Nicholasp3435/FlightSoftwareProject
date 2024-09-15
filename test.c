#include <stdio.h>
#include <stdlib.h>

// Enable stb implementations
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "include/stb_image.h"
#include "include/stb_image_write.h"

int main() {
    int width, height, channels;
    int num_channels = 4;
    
    // Load the PNG image
    unsigned char *img = stbi_load("image.png", &width, &height, &channels, num_channels);
    if (img == NULL) {
        printf("Failed to load image\n");
        return 1;
    }

    printf("Loaded image with width %d, height %d, and %d channels\n", width, height, num_channels);
    
    // Modify pixel data (example: invert colors)
    for (int i = 0; i < width * height * num_channels; i++) {
        img[i] = img[i] & 0b11110000;
    }

    // Write the modified image back to a PNG file
    if (!stbi_write_png("output.png", width, height, num_channels, img, width * num_channels)) {
        printf("Failed to save image\n");
        return 1;
    }

    // Free the image memory
    stbi_image_free(img);

    printf("Image processing complete\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <netpbm/pbm.h>
#include <netpbm/pgm.h>

#define MAX_PIXEL_VALUE 255

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s WxH [R,C=X|X]...\n", argv[0]);
        return 1;
    }

    // Parse width and height from the first command line argument
    char *size_str = strtok(argv[1], "x");
    if (size_str == NULL) {
        printf("Error: Invalid size argument\n");
        return 1;
    }
    int width = atoi(size_str);
    size_str = strtok(NULL, "x");
    if (size_str == NULL) {
        printf("Error: Invalid size argument\n");
        return 1;
    }
    int height = atoi(size_str);

    // Initialize image with all pixels set to 0
    bit **image = pbm_allocarray(width, height);
    if (image == NULL) {
        printf("Error: Failed to allocate image\n");
        return 1;
    }

    // Parse and apply command line arguments
    for (int i = 2; i < argc; i++) {
        char *arg = argv[i];

        if (strchr(arg, ',') != NULL) {
            // Set pixel value
            char *pos = strchr(arg, '=');
            if (pos == NULL) {
                printf("Error: Invalid pixel value argument\n");
                return 1;
            }
            *pos = '\0';
            int row = atoi(strtok(arg, ","));
            int col = atoi(strtok(NULL, ","));
            int value = atoi(pos + 1);
            if (row < 0 || row >= height || col < 0 || col >= width || value < 0 || value > MAX_PIXEL_VALUE) {
                printf("Error: Invalid pixel coordinates or value\n");
                return 1;
            }
            image[row][col] = value;
        } else {
            // Fill entire image with value
            int value = atoi(arg);
            if (value < 0 || value > MAX_PIXEL_VALUE) {
                printf("Error: Invalid fill value\n");
                return 1;
            }
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    image[row][col] = value;
                }
            }
        }
    }

    // Write image to file
    FILE *output;

    output = fopen("image.pgm", "w");
    if (output == NULL) {
        printf("Error: Failed to open file: %s\n", strerror(errno));
        return 1;
    }
    pbm_writepbm(output, image, width, height, 0);
    fclose(output);

    pbm_freearray(image, height);
    return 0;
}

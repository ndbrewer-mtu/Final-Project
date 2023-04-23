#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <netpbm/pbm.h>
#include <netpbm/pgm.h>



int parse_size(const char *arg, int *width, int *height) {
    char *endptr;
    const char *ptr = strchr(arg, 'x');
    if (ptr == NULL || ptr == arg || *(ptr + 1) == '\0') {
        return -1;
    }
    *width = strtol(arg, &endptr, 10);
    if (*endptr != 'x') {
        return -1;
    }
    *height = strtol(ptr + 1, &endptr, 10);
    if (*endptr != '\0') {
        return -1;
    }
    return 0;
}

int parse_pixel(const char *arg, int *row, int *col, unsigned char *value) {
    char *endptr;
    const char *ptr = strchr(arg, ',');
    if (ptr == NULL || ptr == arg || *(ptr + 1) == '\0') {
        return -1;
    }
    *row = strtol(arg, &endptr, 10);
    if (*endptr != ',') {
        return -1;
    }
    *col = strtol(ptr + 1, &endptr, 10);
    if (*endptr != '=') {
        return -1;
    }
    *value = strtol(endptr + 1, &endptr, 10);
    if (*endptr != '\0' || *value > 255) {
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int width, height;
    unsigned char fill_value = 0;
    int i, row, col;
    unsigned char value;
    FILE *output;
    gray **image;

    if (argc < 2 || parse_size(argv[1], &width, &height) != 0) {
        fprintf(stderr, "Usage: %s WxH [R,C=X|X ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    image = pbm_allocarray(width, height);

    if (image == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return EXIT_FAILURE;
    }

    for (i = 2; i < argc; i++) {
        if (isdigit(argv[i][0])) {
            if (sscanf(argv[i], "%hhu", &fill_value) != 1) {
                fprintf(stderr, "Invalid fill value: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
            pbm_clear(image, width, height, fill_value);
        } else {
            if (parse_pixel(argv[i], &row, &col, &value) != 0) {
                fprintf(stderr, "Invalid pixel value: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
            if (row < 0 || row >= height || col < 0 || col >= width) {
                fprintf(stderr, "Pixel out of range: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
            image[row][col] = value;
        }
    }

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

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* BMP dosyalarında tipik olarak BGR sırası kullanılır */
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

typedef struct {
    uint32_t offset;    // Pixel veriye başlangıç offset’i
    uint8_t *header;    // BMP header’ı
    uint32_t width;     // Resmin genişliği
    uint32_t height;    // Resmin yüksekliği
    Pixel *pixels;      // Pixel dizisi
} Image;

Image *load_image(const char *filename);
void save_image(const Image *img, const char *filename);
void free_image(Image *img);
int test();

#endif

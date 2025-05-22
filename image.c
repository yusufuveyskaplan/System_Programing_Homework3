#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "image.h"

/*
 * BMP dosyasının 10. baytındaki (pixel veriye başlangıç offset’i)
 * değeri okuyup, header için hafıza ayırır.
 */
int read_header(FILE *file, Image *img) {
    fseek(file, 10, SEEK_SET);
    fread(&img->offset, 4, 1, file);
    img->header = malloc((img->offset + 4) * sizeof(uint8_t));
    if (!img->header) {
        perror("header array allocation failed");
        return -1;
    }
    rewind(file);
    fread(img->header, sizeof(uint8_t), img->offset, file);
    return img->offset;
}

/* Açık BMP dosyasından resmi okur */
Image *read_image(FILE *file) {
    Image *img = malloc(sizeof(Image));
    if (!img) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    if (read_header(file, img) < 0) {
        perror("could not read header");
        return NULL;
    }
    // DIB header’ın 18. ve 22. baytlarında bulunan genişlik ve yükseklik bilgilerini oku
    uint32_t width, height;
    fseek(file, 18, SEEK_SET);
    fread(&width, 4, 1, file);
    fread(&height, 4, 1, file);

    img->width = width;
    img->height = height;
    img->pixels = malloc(width * height * sizeof(Pixel));
    if (!img->pixels) {
        perror("Pixel array allocation failed");
        return NULL;
    }

    // Pixel verisini oku
    fseek(file, img->offset, SEEK_SET);
    fread(img->pixels, sizeof(Pixel), width * height, file);

    return img;
}

Image *load_image(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    Image *img = read_image(file);
    fclose(file);
    return img;
}

/** Resim belleğini serbest bırakır */
void free_image(Image *img) {
    if (img != NULL) {
        if (img->header != NULL) {
            free(img->header);
        }
        if (img->pixels != NULL) {
            free(img->pixels);
        }
        free(img);
    }
}

void save_image(const Image *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create file");
        return;
    }

    /*
     * (Basitlik açısından, orijinal BMP header’ı kopyalıyoruz.
     * Gerçek uygulamada, header’ı doğru şekilde oluşturmanız gerekir.)
     */
    printf("saving img...!\n");
    rewind(file);
    fwrite(img->header, sizeof(uint8_t), img->offset, file);

    // Pixel verisini yaz
    rewind(file);
    fseek(file, img->offset, SEEK_SET);
    fwrite(img->pixels, sizeof(Pixel), img->width * img->height, file);

    fclose(file);
}

int test() {
    printf("testing img functions\n");
    Image *img = load_image("cat.bmp");
    if (!img) {
        fprintf(stderr, "Failed to load img\n");
        return 1;
    }

    save_image(img, "output.bmp");
    printf("Image saved successfully!\n");

    free_image(img);
    return 0;
}

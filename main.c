#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "image.h"
#include "lsbstegan.h"

#define BIT_PER_CHANNEL 1  

int main() {
    // cat.bmp dosyasını yükle
    Image *img = load_image("cat.bmp");
    if (!img) {
        fprintf(stderr, "Failed to load image\n");
        return 1;
    }

    // Gizlenecek mesaj
    const char *message = "Hello, AI!";
    if (encode_message(img, message, BIT_PER_CHANNEL) != 0) {
        fprintf(stderr, "Message encoding failed\n");
        free_image(img);
        return 1;
    }

    // Değiştirilmiş resmi kaydet
    save_image(img, "encoded.bmp");

    // Mesajı geri oku
    char *decoded = decode_message(img, BIT_PER_CHANNEL);
    if (!decoded) {
        fprintf(stderr, "Message decoding failed\n");
        free_image(img);
        return 1;
    }
    printf("Decoded message: %s\n", decoded);

    // Hafızayı temizle
    free(decoded);
    free_image(img);
    return 0;
}

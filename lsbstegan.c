#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "lsbstegan.h"

/*
 * byte değişkeninin en düşük n-bit’ini (nbit_per_channel) 
 * bits parametresindeki değer ile değiştirir.
 */
uint8_t set_lsb(uint8_t byte, uint8_t bits, uint8_t nbit_per_channel) {
    uint8_t mask = (1 << nbit_per_channel) - 1;
    byte &= ~(mask);              // n LSB’yi sıfırla
    byte |= (bits & mask);        // Yeni bitleri yerleştir
    return byte;
}

/*
 * byte değişkeninin en düşük n-bit’ini döndürür.
 */
uint8_t get_lsb(uint8_t byte, uint8_t nbit_per_channel) {
    uint8_t mask = (1 << nbit_per_channel) - 1;
    return byte & mask;
}

/*
 * Mesajı, resmin her pikselindeki (blue, green, red) kanalların
 * en düşük n-bit’lerine sırayla gizler.
 * Mesajın sonunda null karakter de kodlanır.
 */
int encode_message(Image *img, const char *message, uint8_t nbit_per_channel) {
    uint32_t total_pixels = img->width * img->height;
    uint32_t total_channels = total_pixels * 3; // her pikselde 3 kanal vardır
    uint32_t total_available_bits = total_channels * nbit_per_channel;

    size_t message_length = strlen(message) + 1; // null terminator dahil
    uint32_t message_bits = message_length * 8;

    if (message_bits > total_available_bits) {
        fprintf(stderr, "Message is too long to hide in the image.\n");
        return -1;
    }

    uint32_t bit_index = 0;
    for (uint32_t i = 0; i < total_pixels && bit_index < message_bits; i++) {
        Pixel *p = &img->pixels[i];
        // Sırasıyla: blue, green, red kanallarında sakla
        for (int channel = 0; channel < 3 && bit_index < message_bits; channel++) {
            uint8_t bits_to_hide = 0;
            for (uint8_t b = 0; b < nbit_per_channel; b++) {
                if (bit_index < message_bits) {
                    uint32_t byte_index = bit_index / 8;
                    uint8_t bit_offset = 7 - (bit_index % 8); // her bayttaki MSB'den başla
                    uint8_t bit_val = (message[byte_index] >> bit_offset) & 0x01;
                    bits_to_hide = (bits_to_hide << 1) | bit_val;
                    bit_index++;
                } else {
                    bits_to_hide = (bits_to_hide << 1);
                }
            }
            // Kanalın LSB kısmına gizlenecek bitleri yerleştir
            switch(channel) {
                case 0:
                    p->blue = set_lsb(p->blue, bits_to_hide, nbit_per_channel);
                    break;
                case 1:
                    p->green = set_lsb(p->green, bits_to_hide, nbit_per_channel);
                    break;
                case 2:
                    p->red = set_lsb(p->red, bits_to_hide, nbit_per_channel);
                    break;
            }
        }
    }
    return 0;
}


char *decode_message(const Image *img, uint8_t nbit_per_channel) {
    uint32_t total_pixels = img->width * img->height;
    uint32_t total_channels = total_pixels * 3;
    uint32_t total_available_bits = total_channels * nbit_per_channel;

    // Maksimum mesaj uzunluğu (bayt olarak)
    uint32_t max_message_length = (total_available_bits / 8) + 1;
    char *message = malloc(max_message_length);
    if (!message) {
        return NULL;
    }

    uint32_t bit_index = 0;
    uint32_t message_index = 0;
    uint8_t current_char = 0;
    uint8_t bits_collected = 0;

    for (uint32_t i = 0; i < total_pixels && bit_index < total_available_bits; i++) {
        const Pixel *p = &img->pixels[i];
        for (int channel = 0; channel < 3 && bit_index < total_available_bits; channel++) {
            uint8_t bits = 0;
            switch(channel) {
                case 0:
                    bits = get_lsb(p->blue, nbit_per_channel);
                    break;
                case 1:
                    bits = get_lsb(p->green, nbit_per_channel);
                    break;
                case 2:
                    bits = get_lsb(p->red, nbit_per_channel);
                    break;
            }
            // n-bit içerisindeki her biti sırayla oku (MSB’den başlayarak)
            for (int b = nbit_per_channel - 1; b >= 0; b--) {
                uint8_t bit_val = (bits >> b) & 0x01;
                current_char = (current_char << 1) | bit_val;
                bits_collected++;
                if (bits_collected == 8) {
                    message[message_index++] = current_char;
                    if (current_char == '\0') {
                        return message;
                    }
                    current_char = 0;
                    bits_collected = 0;
                    if (message_index >= max_message_length - 1) {
                        max_message_length *= 2;
                        char *temp = realloc(message, max_message_length);
                        if (!temp) {
                            free(message);
                            return NULL;
                        }
                        message = temp;
                    }
                }
            }
            bit_index += nbit_per_channel;
        }
    }
    // Null terminator ekle (eğer mesajın sonu bulunamadıysa)
    message[message_index] = '\0';
    return message;
}

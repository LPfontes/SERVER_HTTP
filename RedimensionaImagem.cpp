#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize2.h"
#include <iostream>

void redimensiona_imagem(const char* inputPath, const char* outputPath,bool type) {
    int w, h, channels;

    // Carrega a imagem original
    unsigned char* inputImage = stbi_load(inputPath, &w, &h, &channels, 0);
    if (!inputImage) {
        std::cerr << "Erro ao carregar imagem: " << inputPath << "\n";
        return;
    }

    // Calcula o novo tamanho (dobro)
    int newW = w * 2;
    int newH = h * 2;

    // Aloca buffer para imagem redimensionada
    unsigned char* outputImage = new unsigned char[newW * newH * channels];

    // Define o layout de pixels (RGB ou RGBA)
    stbir_pixel_layout layout;
    if (channels == 3)
        layout = STBIR_RGB;
    else if (channels == 4)
        layout = STBIR_RGBA;
    else {
        std::cerr << "Número de canais não suportado: " << channels << "\n";
        stbi_image_free(inputImage);
        delete[] outputImage;
        return;
    }

    // Redimensiona com correção sRGB
    stbir_resize_uint8_srgb(
        inputImage, w, h, 0,
        outputImage, newW, newH, 0,
        layout
    );

    // Salva como JPEG
    if (type) { 
        stbi_write_png(outputPath, newW, newH, channels, outputImage, newW * channels);
    } else {
        stbi_write_jpg(outputPath, newW, newH, channels, outputImage, 100);
    }
    // Libera recursos
    stbi_image_free(inputImage);
    delete[] outputImage;

    std::cout << "Imagem redimensionada salva em: " << outputPath << "\n";
}

#include <stdio.h>
#include <stdlib.h>

//#include "player.h"
//#include "gameLoop.h"
//#include "runDBuff.h"
#include "globals.h"

//int iffTexture[64][64]={0};

void textureFileOpen() {

#define BMP_HEADER_SIZE 54
#define BMP_PALETTE_SIZE 16
#define BMP_PIXELS_SIZE 2048

	char filename="txt64x64.bmp";
    FILE *file = fopen("txt64x64.bmp", "rb");

    if (file == NULL) {
        printf("Nie mo¿na otworzyæ pliku %s\n", filename);
        return;
    }

    // Przeskocz nag³ówek BMP i paletê kolorów
    fseek(file, BMP_HEADER_SIZE+64, SEEK_SET);

    // Odczytaj dane pikseli (indeksy kolorów)
    uint8_t buffer[BMP_PIXELS_SIZE] = {0};
    fread(buffer, sizeof(uint8_t), BMP_PIXELS_SIZE, file);

    // Zapisz indeksy do tablicy iffTexture
    int pixelIndex = 0;
    int progress=0;
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64 / 2; x++) {


        	if (progress==(32)){ progress=0;}

            uint8_t pixelData = buffer[pixelIndex];
            uint8_t pixel1 = (pixelData >> 4)& 0x0F; // Piksel z lewej strony
            uint8_t pixel2 = pixelData & 0x0F;        // Piksel z prawej strony
            iffTexture[x +progress][y] = pixel1;
            iffTexture[x +progress + 1][y] = pixel2;
            pixelIndex++;
            progress++;
        }
    }

    fclose(file);
    return ;
}

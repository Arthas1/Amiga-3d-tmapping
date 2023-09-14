#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdio.h>
#include <exec/types.h>
#include <intuition/intuition.h>


extern int iffTexture[64][64];
extern ULONG windowSignalGame;

typedef struct {
    int model;
    int pos_x, pos_y, pos_z;
    float rot_x, rot_y, rot_z;
    // Dodaj inne pola gracza, jeœli s¹ potrzebne
} Player;

typedef struct {
    int x, y, z, w;
} Vertex;

typedef struct {
    float u, v, w;
} TexUV;

typedef struct {
    int c0, c1, c2, c3, c4, c5;
} Faces;

typedef struct {
    int vert, uvs, fejs;
} Counters;


#endif /* GLOBALS_H_ */

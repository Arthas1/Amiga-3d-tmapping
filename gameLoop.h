#ifndef GAMELOOP_H_
#define GAMELOOP_H_


#include <stdio.h>

//#include "nodeFileOpen.h"
//#include "modelFileOpen.h"
//#include "textureFileOpen.h"

//#include "WindowGetSignal.h"
//#include "globals.h"
//#include "runDBuffT.h"

//extern Player *playerArray;
//extern Vertex *vertices;
//extern TexUV *texUV;
//extern Faces *faces;
//extern ULONG windowSignalGame;
//extern Counters *counters;

extern int vertexCount;
extern int uvCount;
extern int faceCount;

void gameLoop(struct RastPort *rp, struct Screen *screen, struct BitMap **myBitMaps, struct Window *win);


#endif /* GAMELOOP_H_ */


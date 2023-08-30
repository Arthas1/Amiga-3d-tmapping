#include <stdio.h>
#include <intuition/intuition.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <clib/utility_protos.h>
#include <proto/dos.h>
#include <devices/timer.h>
#include <stdlib.h>
#include <clib/timer_protos.h>
#include "quicksort.h"
#include <string.h>
//#include "fileOpen.h"

//#define PI 3.1415


// timeb
struct Device *TimerBase;
struct Library *UtilityBase;

/* characteristics of the screen */
// zmienic viewmode = 0 - lores, zmniejszyc model zeby nie rysowal poza ekranem

#define SCR_WIDTH  (320)		// SET RESOLUTION - 320x200; 320x400; 640x200; 640x400
#define SCR_HEIGHT (200)
#define SCR_DEPTH    (4)

#define MAX_VERTICES	1000
#define MAX_FACES		1000


//
/* Prototypes for our functions */
//VOID runDBuff(struct Screen*, struct BitMap**);
VOID runDBuff(struct Screen*, struct BitMap**, struct Window *win);
struct BitMap** setupBitMaps(LONG, LONG, LONG);
VOID freeBitMaps(struct BitMap**, LONG, LONG, LONG);
LONG setupPlanes(struct BitMap*, LONG, LONG, LONG);
VOID freePlanes(struct BitMap*, LONG, LONG, LONG);




//int ReadMesh(void);

struct Library *IntuitionBase = NULL;
struct Library *GfxBase = NULL;

int vertices[MAX_VERTICES][4] = {0};

// float vertices[MAX_VERTICES][4] = {0};
int faces[MAX_FACES][6] = {0};

int nrVertices;
float texUV[MAX_VERTICES][3]={0};
//int texUV[MAX_VERTICES][3]={0};
int texs[MAX_FACES][2]={0};
int nrUV;
int hasTexture = 0;

int palette[16][4];
int widthTxt;
int heightTxt;

int nrFaces = 0;
//int vertexNr;
int x, y, z;

int CLEAR =1;

//float w=1;
//float wt=1;


int w=1;
int wt=1;

int xClip1;
int yClip1;
int xClip2;
int yClip2;
int xClip3;
int yClip3;

int pDepthBuffer[]={0};
int scrSize=SCR_WIDTH*SCR_HEIGHT;
int sampleColour;

int iffTexture[64][64]={0};

//UBYTE arrayScreen[SCR_WIDTH*SCR_HEIGHT]={0};

//arrayScreen = AllocMem(SCR_WIDTH * SCR_HEIGHT * 4, MEMF_CHIP | MEMF_CLEAR);
//arrayScreen= AllocMem(SCR_WIDTH*SCR_HEIGHT*4,MEMF_ANY);

// ------------------------------------------------------------------------------------------------
// print float to provided string buffer

char * ftoa(float f, char *b) {
	sprintf(b, "%s%d.%d",(f<0)?"-":"",abs((int)f),abs((int)((f - (int)f)*1000000)));
	return b;
}

// pre-calculated sin and cos implementation

float sintab[91] = {0.000000,0.017452,0.034899,0.052336,0.069756,0.087156,0.104528,0.121869,0.139173,0.156434,0.173648,0.190809,0.207912,0.224951,0.241922,0.258819,0.275637,0.292372,0.309017,0.325568,0.342020,0.358368,0.374607,0.390731,0.406737,0.422618,0.438371,0.453990,0.469472,0.484810,0.500000,0.515038,0.529919,0.544639,0.559193,0.573576,0.587785,0.601815,0.615661,0.629320,0.642788,0.656059,0.669131,0.681998,0.694658,0.707107,0.719340,0.731354,0.743145,0.754710,0.766044,0.777146,0.788011,0.798636,0.809017,0.819152,0.829038,0.838671,0.848048,0.857167,0.866025,0.874620,0.882948,0.891007,0.898794,0.906308,0.913545,0.920505,0.927184,0.933580,0.939693,0.945519,0.951057,0.956305,0.961262,0.965926,0.970296,0.974370,0.978148,0.981627,0.984808,0.987688,0.990268,0.992546,0.994522,0.996195,0.997564,0.998630,0.999391,0.999848,1.0};

// input: angle in degrees
float ksin(int x) {
	if (x<0) x=(-x+180);
	if (x>=360) x%=360;
	if (x<=90) {
		return sintab[x];
	} else if (x<=180) {
		return sintab[180-x];
	} else if (x<=270) {
		return -sintab[x-180];
	} else {
		return -sintab[360-x];
	}
}

// input: angle in degrees
float kcos(int x) {
	return ksin(x+90);
}
// ------------------------------------------------------------------------------------------------

/*
 setupBitMaps(): allocate the bit maps for a double buffered screen.
 */

struct BitMap** setupBitMaps(LONG, LONG, LONG);
struct BitMap** setupBitMaps(LONG depth, LONG width, LONG height) {
/* this must be static -- it cannot go away when the routine exits. */
static struct BitMap *myBitMaps[2];

myBitMaps[0] = (struct BitMap*) AllocMem((LONG) sizeof(struct BitMap),	MEMF_CHIP |	MEMF_CLEAR);

if (myBitMaps[0] != NULL) {
	myBitMaps[1] = (struct BitMap*) AllocMem((LONG) sizeof(struct BitMap),	MEMF_CHIP |	MEMF_CLEAR);

	if (myBitMaps[1] != NULL) {
		InitBitMap(myBitMaps[0], depth, width, height);
		InitBitMap(myBitMaps[1], depth, width, height);

		if (0 != setupPlanes(myBitMaps[0], depth, width, height)) {
			if (0 != setupPlanes(myBitMaps[1], depth, width, height))
				return (myBitMaps);

			freePlanes(myBitMaps[0], depth, width, height);
		}
		FreeMem(myBitMaps[1], (LONG) sizeof(struct BitMap));
	}
	FreeMem(myBitMaps[0], (LONG) sizeof(struct BitMap));
}
return (NULL);
}

/*
 ** runDBuff(): loop through a number of iterations of drawing into
 ** alternate frames of the double-buffered screen.  Note that the

 */
 VOID runDBuff(struct Screen *screen, struct BitMap **myBitMaps, struct Window *win ) {

//	VOID runDBuff(struct Screen *screen, struct BitMap **myBitMaps) {
//------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	float x_orig, y_orig, z_orig, x1, x2,x3, y1, y2, y3, z1, z2, z3;
//	float u_orig, v_orig, w_orig, x1t, x2t,x3t, y1t, y2t, y3t, z1t, z2t, z3t;


	int x_orig, y_orig, z_orig, x1, x2,x3, y1, y2, y3, z1, z2, z3;
	int u_orig, v_orig, w_orig, x1t, x2t,x3t, y1t, y2t, y3t, z1t, z2t, z3t;



	int result[MAX_VERTICES][6] = {0};

	// Projection Matrix

//	float fNear = 0.1f;

float fFar = 100000.0f;    // od tego zalezy ogniskowa  (1000 dla float)


	float fFov = 150.0f;
	float fAspectRatio = 1.5f;
//	float fFovRad = 1.0f;

	float modifierX=SCR_WIDTH*0.0005f;								// 0.2 -> 320 width
	float modifierY=SCR_HEIGHT*0.0013f ;





	struct AreaInfo ainfo = {0};
	 WORD areabuf[SCR_WIDTH];
	struct TmpRas Tmp;
	APTR tmpbuf;
	int ktr;
	WORD toggleFrame;


	struct RastPort *rport;
	rport = &screen->RastPort;
	int frameNo = 0;

	rport->AreaInfo = &ainfo;

	int fps;

	struct ClockData *clockdata;
	struct timerequest *tr;
	struct timeval *tv;

	LONG dilatationTime;
	LONG startTime;
	LONG endTime;


	ULONG winSignal = 1L << win->UserPort->mp_SigBit;


	int pixel_bw=0;

	tmpbuf = (APTR)AllocMem(SCR_WIDTH * SCR_HEIGHT * 4, MEMF_CHIP | MEMF_CLEAR) ;  /* return with warning if unable to alloc memory */
    InitTmpRas(&Tmp, tmpbuf,SCR_WIDTH * SCR_HEIGHT * 4);
    rport->TmpRas = &Tmp;

     // INICJALIZACJA TIMER

if (UtilityBase = OpenLibrary("utility.library", 37)) {
	if (tr = AllocMem(sizeof(struct timerequest), MEMF_CLEAR)) {
		if (tv = AllocMem(sizeof(struct timeval), MEMF_CLEAR)) {
			if (clockdata = AllocMem(sizeof(struct ClockData), MEMF_CLEAR)) {
				if (!(OpenDevice("timer.device", UNIT_VBLANK,
						(struct IORequest*) tr, 0))) {
					TimerBase = tr->tr_node.io_Device;

					GetSysTime(tv);                     // check initial time
					startTime = tv->tv_micro;



					//char str[123];
					char str1[123];

					char slot1[123], slot2[123], slot3[123], slot4[123], slot5[123];

					int angles[3][2] = {{0,-3}, {0,3}, {0,0}};  // for all planes: { current angle, rotation speed }
					int angle;

					/* MAIN DRAWING LOOP NA 2 BUFORY EKRANU ----------------------------------------------------- */

					SetRast(&(screen->RastPort), 0);
					toggleFrame = 0;


//----------------------------------------------------------------------LOOP FOR ktr = number of frames-----------------------------------------------------------------------------------------------------
					for (ktr = 0; ktr <50; ktr++) {




				//		handleWinSignal(win);



						/* switch the bitmap so that we are drawing into the correct place */
						screen->RastPort.BitMap = myBitMaps[toggleFrame];
						screen->ViewPort.RasInfo->BitMap =	myBitMaps[toggleFrame];

					//	SetAPen(rport, 1);

						// najpierw obroc wszystkie trojkaty //

						for (int k=0; k<nrVertices; k++) {

							// odczytaj po jednym wierzcholku / read vertice by vertice

								x_orig = vertices[k][0];
								y_orig = vertices[k][1];
								z_orig = vertices[k][2];

						u_orig = texUV[k][0];
						v_orig = texUV[k][1];
						w_orig = texUV[k][2];


// usunac potem: przesun bryle tak, zeby srodek obrotu byl w srodku a nie w jednym rogu / it is for moving axis of rotation

//x_orig -=0.5f; // Lokalizacja kamery / camera usable
// y_orig +=50;
//z_orig -=150;

								angle = angles[0][0];
								x1 = x_orig*kcos(angle) - y_orig*ksin(angle);
								y1 = x_orig*ksin(angle) + y_orig*kcos(angle);
								z1 = z_orig;

								x1t = u_orig*kcos(angle) - v_orig*ksin(angle);	// texture related
								y1t = v_orig*ksin(angle) + v_orig*kcos(angle);
								z1t = w_orig;

								angle = angles[1][0];
								x2 = x1*kcos(angle) - z1*ksin(angle);
								y2 = y1;
								z2 = x1*ksin(angle) + z1*kcos(angle);

								x2t = x1t*kcos(angle) - z1t*ksin(angle);
								y2t = y1t;
								z2t = x1t*ksin(angle) + z1t*kcos(angle);


								angle = angles[2][0];
								x3 = x2;
								y3 = y2*kcos(angle) - z2*ksin(angle);
								z3 = y2*ksin(angle) + z2*kcos(angle);

								angle = angles[2][0];
								x3t = x2t;
								y3t = y2t*kcos(angle) - z2t*ksin(angle);
								z3t = y2t*ksin(angle) + z2t*kcos(angle);

								// perspektywa - nieco uproszczona / simplified perspective

								x3 = x3*(fFar-z3*fFov)/fFar;
								y3 = y3*(fFar-z3*fFov)/fFar;

								x3t = x3t*(fFar-z3t*fFov)/fFar;	// texel
								y3t = y3t*(fFar-z3t*fFov)/fFar;

								// wysrodkuj na ekranie i uwzglednij aspekt ratio i zapamietaj odwrocone trojkaty

								result[k][0] = x3;
								result[k][1] = y3;
								result[k][2] = z3;

								result[k][3] = x3t;
								result[k][4] = y3t;
								result[k][5] = z3t;

						}

						// zupdatuj wszystkie katy obrotu / update all angles
						for (int t=0; t<3; t++) {

							angles[t][0] += angles[t][1];
						}


// QUICK SORTING - sorting of all polygons over Z

		int number[MAX_FACES]; // tyle co ogolna liczba trojkatow w scenie / max triangles in scene
		int faceindex[MAX_FACES]; // tu beda posortowane indeksy fejsow / sorted out triangle indexes
		int nrNormalFaces=0;

int X1, X2, X3, Y1,Y2,Y3;


int clipTest=0;
int t1;  // test bits
int t2;
int t3;

for (int e = 0 ; e < nrFaces; e++){






												    		 int referSort1 = faces[e][0];
												    		 int referSort2 = faces[e][1];
												    		 int referSort3 = faces[e][2];

												    		 int zBuff = (result[referSort1][2] + result[referSort2][2] + result[referSort3][2]) / 3.0f;

												    		 zBuff*=10;

												    		number[e]=  (int)zBuff;
															faceindex[e] = e;
									 }


quicksort(number, faceindex, 0, nrFaces-1);


// Draw every triangle of the array / narysuj wszystkie trojkaty z tablicy


						/* Draw the objects --------------------------------------------------------------
						 ** Here we clear the old frame and draw a triangles.						*/

						SetRast(rport, 0);

						for (int k=0; k<nrFaces; k++) {


							 // Normalise polygons

							int referPoint1 = faces[faceindex[k]][0];		//Ver1 referPoint to pozycja porzadkowa wierzcholka na liscie faces ktora przechowuje x, y, z od poligonu o indeksie k
							int referPoint2 = faces[faceindex[k]][1];		//Ver2
							int referPoint3 = faces[faceindex[k]][2];		//Ver3

							int line1x = result[referPoint2][0] - result[referPoint1][0];
							int line1y = result[referPoint2][1] - result[referPoint1][1];
							int line1z = result[referPoint2][2] - result[referPoint1][2];

							int line2x = result[referPoint3][0] - result[referPoint1][0];
							int line2y = result[referPoint3][1] - result[referPoint1][1];
							int line2z = result[referPoint3][2] - result[referPoint1][2];

							// Dot product
					//		float normalx = line1y * line2z - line1z * line2y;
					//		float normaly = line1z * line2x - line1x * line2z;
							int normalz = line1x * line2y - line1y * line2x;


					//		int tMapRefferPoint1 = faces[faceindex[k]][3]; // wskazanie indexu wierzcholka / vertice index
					//		int tMapRefferPoint2 = faces[faceindex[k]][4];
					//		int tMapRefferPoint3 = faces[faceindex[k]][5];


					//		printf ("faceindex: %d, %d, %d\n", referPoint1, referPoint2, referPoint3);
					//		printf ("tex refer : %d, %d, %d\n", tMapRefferPoint1, tMapRefferPoint2, tMapRefferPoint3);


							// Check if polygon is normal / Sprawdz czy Poligon Normalny  - gdy normalz<0

							if(normalz < 0.0f) {
								nrNormalFaces++;

								// Lighting of normal polygons / Oswietlenie poligonow normalnych / tylko

	//							float light_direction[3] = {-1.5f, -0.5f,  -1.0f };

		//						float l=1;

		//						light_direction[0] /= l; light_direction[1] /= l; light_direction[2] /= l;

								// dp = normalx * light_direction[0] + normaly * light_direction[1] + normalz * light_direction[2];

								// pixel_bw = (int)(1*dp*9.1f);  //pixel_bw = (int)(25*dp*1.3f);


//						pixel_bw =1;
//								if (pixel_bw<1){pixel_bw =1;}
//								if (pixel_bw>14){pixel_bw =15;}


								// Read UVW data of texture / Odczytaj dane UVW textury

								int tMapRefferPoint1 = faces[faceindex[k]][3]; // pointing index of vertice
								int tMapRefferPoint2 = faces[faceindex[k]][4];
								int tMapRefferPoint3 = faces[faceindex[k]][5];

								 float p1mapU = texUV[tMapRefferPoint1][0];
								 float p1mapV = texUV[tMapRefferPoint1][1];
								 float p1mapW = texUV[tMapRefferPoint1][2];

								 float p2mapU = texUV[tMapRefferPoint2][0];
								 float p2mapV = texUV[tMapRefferPoint2][1];
								 float p2mapW = texUV[tMapRefferPoint2][2];

								 float p3mapU = texUV[tMapRefferPoint3][0];
								 float p3mapV = texUV[tMapRefferPoint3][1];
								 float p3mapW = texUV[tMapRefferPoint3][2];



								int xm, xm2, xm3 ; int ym, ym2 , ym3;



								 InitArea(&ainfo, areabuf, SCR_HEIGHT *2/4);

							//	SetOutlinePen(rport, pixel_bw-4);    					// FOR EDGES
				SetAPen(rport,pixel_bw);
				// KOREKTA EKRANOWA
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

								int x1Projected=(result[referPoint1][0] +400 )*modifierX;								// 0.2 -> 320 width
								int y1Projected=(result[referPoint1][1] +300 )/fAspectRatio * modifierY ;				//0.3 -> 200 width
								int x2Projected=(result[referPoint2][0] +400 )*modifierX;
								int y2Projected=(result[referPoint2][1] +300 )/fAspectRatio * modifierY;
								int x3Projected=(result[referPoint3][0] +400 ) *modifierX;
								int y3Projected=(result[referPoint3][1] +300 )/fAspectRatio * modifierY;

//=================================================================================HERE STARTS DRAWING / TU ZACZYNA SIE RYSOWANIE==================================================================================

								//	CLIPPING Cohen–Sutherland algorithm ???
								//   slope = (y1 - y0) / (x1 - x0)
								//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
								//   y = y0 + slope * (xm - x0), where xm is xmin or xmax


								// CASE TEST - ONLY FILLED VECTORS AT THE MOMENT

									if ( x1Projected<0 || x1Projected>=SCR_WIDTH || y1Projected<0 || y1Projected>=SCR_HEIGHT){ t1=1;	} else { t1=0;}

									if ( x2Projected<0 || x2Projected>=SCR_WIDTH || y2Projected<0 || y2Projected>=SCR_HEIGHT){ t2=1;	} else { t2=0;}

									if ( x3Projected<0 || x3Projected>=SCR_WIDTH || y3Projected<0 || y3Projected>=SCR_HEIGHT){ t3=1;	} else { t3=0;}

									clipTest= t1+t2+t3;

								//==============================================clipping============================================================================================================
									X1= x1Projected; X2=x2Projected; X3=x3Projected; Y1=y1Projected; Y2= y2Projected; Y3=y3Projected;



// case 1 - TYLKO JEDEN PUNKT ZA EKRANEM

if ( clipTest==1 ) {

									if( y1Projected<0 || y1Projected>=SCR_HEIGHT ){

										if (y1Projected<0 ) {ym=0;} else ym= SCR_HEIGHT-1;
								xClip1 = X1 + (X2 - X1) * (ym - Y1) / (Y2 - Y1);   // 0 - minimalna ekrany
								if (y1Projected>=SCR_HEIGHT-1) {yClip1=SCR_HEIGHT-1;} else { yClip1 = 0;}


								xClip2 = X1 + (X3 - X1) * (ym - Y1) / (Y3 - Y1);
								if (y1Projected>=SCR_HEIGHT-1) {yClip2=SCR_HEIGHT-1;} else { yClip2 = 0;}



											AreaMove(rport, xClip1, yClip1);				// NEW vert 1
											AreaDraw(rport, x2Projected, y2Projected);
											AreaDraw(rport, x3Projected, y3Projected);
											AreaEnd(rport);

											AreaMove(rport, xClip1, yClip1);				// NEW vert 2
											AreaDraw(rport, x3Projected, y3Projected);
											AreaDraw(rport, xClip2,yClip2);
											AreaEnd(rport);

										//	printf ("P1 Y OUT %d\n", y1Projected);

}

								else
									if( y2Projected<0 || y2Projected>=SCR_HEIGHT ){
										if (y2Projected<0 ) {ym=0;} else ym= SCR_HEIGHT-1;
											xClip1 = X2 + (X1 - X2) * (ym - Y2) / (Y1 - Y2);   // 0 - minimalna ekrany

											if (y2Projected>=SCR_HEIGHT) {yClip1=SCR_HEIGHT-1;} else { yClip1 = 0;}
											xClip2 = X2 + (X3 - X2) * (ym - Y2) / (Y3 - Y2);
											if (y2Projected>=SCR_HEIGHT) {yClip2=SCR_HEIGHT-1;} else { yClip2 = 0;}

// Rysowanie wektorow
											AreaMove(rport, x1Projected,y1Projected);				// NEW vert 1
											AreaDraw(rport, xClip1, yClip1);
											AreaDraw(rport, xClip2,yClip2);
											AreaEnd(rport);

											AreaMove(rport, x1Projected, y1Projected);				// NEW vert 2
											AreaDraw(rport,  xClip2,yClip2);
											AreaDraw(rport, x3Projected,y3Projected);
											AreaEnd(rport);

									//printf ("P2 Y OUT %d\n", y2Projected);

									}
											else

									if( y3Projected<0 || y3Projected>=SCR_HEIGHT ){
									if (y3Projected<0 ) {ym=0;} else ym= SCR_HEIGHT-1;
									xClip1 = X3 + (X1 - X3) * (ym - Y3) / (Y1 - Y3);   // 0 - minimalna ekrany
									if (y3Projected>=SCR_HEIGHT) {yClip1=SCR_HEIGHT-1;} else { yClip1 = 0;}
									xClip2 = X3 + (X2 - X3) * (ym - Y3) / (Y2 - Y3);
									if (y3Projected>=SCR_HEIGHT) {yClip2=SCR_HEIGHT-1;} else { yClip2 = 0;}


									AreaMove(rport, x1Projected,y1Projected);				// NEW vert 1
									AreaDraw(rport, xClip1, yClip1);
									AreaDraw(rport, xClip2,yClip2);
									AreaEnd(rport);

									AreaMove(rport, x1Projected, y1Projected);				// NEW vert 2
									AreaDraw(rport, x2Projected, y2Projected);
									AreaDraw(rport, xClip2, yClip2);
									AreaEnd(rport);

									//printf ("P3 Y OUT %d\n", y3Projected);

}
									else

									if( x1Projected<0 || x1Projected>=SCR_WIDTH ){

									if (x1Projected<0 ) {xm=0;} else xm= SCR_WIDTH-1;

									yClip1 = Y1 + (Y2 - Y1) * (xm - X1) / (X2 - X1);   // 0 - minimalna ekrany
									yClip2 = Y1 + (Y3 - Y1) * (xm - X1) / (X3 - X1);

									if (x1Projected>=SCR_WIDTH) {xClip1=SCR_WIDTH-1; xClip2=SCR_WIDTH-1;} else { xClip1 = 0; xClip2 = 0;}


									AreaMove(rport, xClip1, yClip1);				// NEW vert 1
									AreaDraw(rport, x2Projected, y2Projected);
									AreaDraw(rport, x3Projected, y3Projected);
									AreaEnd(rport);
									AreaMove(rport, xClip1, yClip1);				// NEW vert 2
									AreaDraw(rport, x3Projected, y3Projected);
									AreaDraw(rport, xClip2, yClip2);
									AreaEnd(rport);

									//printf ("P1 X OUT %d\n", x1Projected);

																			}

									else
									if( x2Projected<0 || x2Projected>=SCR_WIDTH ){

										if (x2Projected<0 ) {xm=0;} else xm= SCR_WIDTH-1;



									yClip1 = Y2 + (Y1 - Y2) * (xm - X2) / (X1 - X2);   // 0 - minimalna ekrany
									if (x2Projected>=SCR_WIDTH) {xClip1=SCR_WIDTH-1;} else { xClip1 = 0;}
									yClip2 = Y2 + (Y3 - Y2) * (xm - X2) / (X3 - X2);
									if (x2Projected>=SCR_WIDTH) {xClip2=SCR_WIDTH-1;} else { xClip2 = 0;}

									AreaMove(rport, x1Projected,y1Projected);				// NEW vert 1
									AreaDraw(rport, xClip1, yClip1);
									AreaDraw(rport, xClip2,yClip2);
									AreaEnd(rport);

									AreaMove(rport, x1Projected, y1Projected);				// NEW vert 2
									AreaDraw(rport,  xClip2,yClip2);
									AreaDraw(rport, x3Projected,y3Projected);
									AreaEnd(rport);

							        //printf ("P2 X OUT %d\n", x2Projected);

								}

									else
									if( x3Projected<0 || x3Projected>=SCR_WIDTH ){

									if (x3Projected<0 ) {xm=0;} else xm= SCR_WIDTH-1;

									yClip1 = Y3 + (Y1 - Y3) * (xm - X3) / (X1 - X3);   // 0 - minimalna ekrany
									if (x3Projected>=SCR_WIDTH) {xClip1=SCR_WIDTH-1;} else { xClip1 = 0;}
									yClip2 = Y3 + (Y2 - Y3) * (xm - X3) / (X2 - X3);
									if (x3Projected>=SCR_WIDTH) {xClip2=SCR_WIDTH-1;} else { xClip2 = 0;}

									AreaMove(rport, x1Projected,y1Projected);				// NEW vert 1
									AreaDraw(rport, xClip1, yClip1);
									AreaDraw(rport, xClip2,yClip2);
									AreaEnd(rport);

									AreaMove(rport, x1Projected, y1Projected);				// NEW vert 2
									AreaDraw(rport,   xClip2,yClip2);
									AreaDraw(rport,  x2Projected, y2Projected);
									AreaEnd(rport);

								//	printf ("P3 X OUT %d\n", x3Projected);

								}

}


// case 2 - 2 POINTS OFF THE SCREEN ---------------------------------------------------------------

if (clipTest==2) {
	// printf ("Podwojny %d\n", xClip2);

	    if (y1Projected<=0 ) {ym=0;} else  {ym= SCR_HEIGHT-1;}
	    if (x1Projected<=0 ) {xm=0;} else  {xm= SCR_WIDTH-1;}

		if (y3Projected<=0 ) {ym3=0;} else {ym3= SCR_HEIGHT-1;}
		if (x3Projected<=0 ) {xm3=0;} else {xm3= SCR_WIDTH-1;}

		if (y2Projected<=0 ) {ym2=0;} else {ym2= SCR_HEIGHT-1;}
		if (x2Projected<=0 ) {xm2=0;} else {xm2= SCR_WIDTH-1;}

// P1 w ekranie

if ( x1Projected>=0 && x1Projected<=SCR_WIDTH-1 && y1Projected>=0 && y1Projected<=SCR_HEIGHT-1 && ( y2Projected<0 || y2Projected>=SCR_HEIGHT || x2Projected<0 || x2Projected>=SCR_WIDTH ) && (y3Projected<0 || y3Projected>=SCR_HEIGHT  || x3Projected<0 || x3Projected>=SCR_WIDTH)){

	if ( x2Projected<0) { xClip1 = 0;} else if ( x2Projected>=SCR_WIDTH-1) { xClip1 = SCR_WIDTH-1; } else {xClip1 = X1 + (X2 - X1) * (ym2 - Y1) / (Y2 - Y1);}   // 0 - minimalna ekrany
	if ( x3Projected<0) { xClip2 = 0;} else if ( x3Projected>=SCR_WIDTH-1) { xClip2 = SCR_WIDTH-1; } else {xClip2 = X1 + (X3 - X1) * (ym3 - Y1) / (Y3 - Y1);}
	if ( y2Projected<0) {yClip1 = 0;} else if ( y2Projected>=SCR_HEIGHT-1) { yClip1 = SCR_HEIGHT-1; } else {yClip1 = Y1 + (Y2 - Y1) * (xm2 - X1) / (X2 - X1); }  // 0 - minimalna ekrany
	if ( y3Projected<0) {yClip2 = 0;} else if ( y3Projected>=SCR_HEIGHT-1) { yClip1 = SCR_HEIGHT-1; } else {yClip2 = Y1 + (Y3 - Y1) * (xm3 - X1) / (X3 - X1);}

	AreaMove(rport, x1Projected, y1Projected);				// NEW vert 1
	AreaDraw(rport, xClip1, yClip1);
	AreaDraw(rport, xClip2, yClip2);
	AreaEnd(rport);

}

// P2 w ekranie

else
if ( x2Projected>=0 && x2Projected<=SCR_HEIGHT-1 && y2Projected>=0 && y2Projected<=SCR_WIDTH-1 &&
		( y1Projected<0 || y1Projected>=SCR_HEIGHT || x1Projected<0 || x1Projected>=SCR_WIDTH ) && (y3Projected<0 || y3Projected>=SCR_HEIGHT  || x3Projected<0 || x3Projected>=SCR_WIDTH) ){


	if ( x1Projected<0) { xClip1 = 0;} else if ( x1Projected>=SCR_WIDTH-1) { xClip1 = SCR_WIDTH-1; } else {xClip1 = X2 + (X1 - X2) * (ym - Y2) / (Y1 - Y2);}   // 0 - minimalna ekrany
	if ( x3Projected<0) { xClip2 = 0;} else if ( x3Projected>=SCR_WIDTH-1) { xClip1 = SCR_WIDTH-1; } else {xClip2 = X2 + (X3 - X2) * (ym3 - Y2) / (Y3 - Y2);}
	if ( y1Projected<0) {yClip1 = 0;} else if ( y1Projected>=SCR_HEIGHT-1) { yClip1 = SCR_HEIGHT-1; } else {yClip1 = Y2 + (Y1 - Y2) * (xm - X2) / (X1 - X2);}   // 0 - minimalna ekrany
	if ( y3Projected<0) {yClip2 = 0;} else if ( y3Projected>=SCR_HEIGHT-1) { yClip1 = SCR_HEIGHT-1; }else {yClip2 = Y2 + (Y3 - Y2) * (xm3 - X2) / (X3 - X2);}


		AreaMove(rport, xClip1, yClip1);				// NEW vert 1
		AreaDraw(rport, x2Projected, y2Projected);
		AreaDraw(rport, xClip2, yClip2);
		AreaEnd(rport);

	//	printf ("P1 and P3 OUT %d\n", x1Projected);

}

// P3 w ekranie

else
if ( x3Projected>=0 && x3Projected<=SCR_HEIGHT-1 && y3Projected>=0 && y3Projected<=SCR_WIDTH-1 &&
		( y1Projected<0 || y1Projected>=SCR_HEIGHT || x1Projected<0 || x1Projected>=SCR_WIDTH ) && (y2Projected<0 || y2Projected>=SCR_HEIGHT  || x2Projected<0 || x2Projected>=SCR_WIDTH) ){


	if ( x1Projected<0) { xClip1 = 0;} else if ( x1Projected>=SCR_WIDTH-1) { xClip1 = SCR_WIDTH-1; } else {xClip1 = X3 + (X1 - X3) * (ym - Y3) / (Y1 - Y3);}   // 0 - minimalna ekrany
	if (x2Projected<0) { xClip2 = 0;} else if ( x2Projected>=SCR_WIDTH-1) { xClip1 = SCR_WIDTH-1; } else {xClip2 = X3 + (X2 - X3) * (ym2 - Y3) / (Y2 - Y3);}
	if ( y1Projected<0) {yClip1 = 0;} else if ( y1Projected>=SCR_HEIGHT-1) { yClip1 = SCR_HEIGHT-1; } else {	yClip1 = Y3 + (Y1 - Y3) * (xm - X3) / (X1 - X3);}   // 0 - minimalna ekrany
	if ( y2Projected<0) {yClip2 = 0;} else if ( y2Projected>=SCR_HEIGHT-1) { yClip1 = SCR_HEIGHT-1; } else {	yClip2 = Y3 + (Y2 - Y3) * (xm2 - X3) / (X2 - X3);}


		AreaMove(rport, xClip1, yClip1);				// NEW vert 1
		AreaDraw(rport, xClip2, yClip2);
		AreaDraw(rport, x3Projected, y3Projected);
		AreaEnd(rport);

//		printf ("P1 and P2 OUT %d\n", x3Projected);
}

// Wszystko poza ekranem
if (clipTest==3) {

}


}


// CASE 3 wszystko na ekranie
if (clipTest<1) {

{
//AreaMove(rport, x1Projected, y1Projected);
//AreaDraw(rport, x2Projected, y2Projected);
//AreaDraw(rport, x3Projected, y3Projected);
//AreaEnd(rport);




//for(int i=0; i<4; i++){
//	for(int j=0; j<4; j++){
//	pixel_bw=iffTexture[i][j];
//		SetAPen(rport,pixel_bw);
//	WritePixel(rport, 50+i, 50+j);
//	}
//}
//WritePixel(rport, x1Projected+20, y1Projected+2);

// TEXTUROWANIE

int x1=x1Projected, x2=x2Projected, x3=x3Projected, y1=y1Projected, y2=y2Projected, y3=y3Projected;
float u1=p1mapU, u2=p2mapU, u3=p3mapU, v1=p1mapV, v2=p2mapV, v3=p3mapV, w1=p1mapW, w2=p2mapW, w3=p3mapW;


// Sprawdzanie wspolrzednych ekranowych i ich odwracanie jesli trzeba y2 i y3 musza byc mniejsze od y1 i odpowiednio y2
// Checking screen coords and swapping if needed. y2 and y3 have to be smaller then y1 and y2

if (y2 < y1)	{
//	swap(y1, y2);
int temp = y1;
y1 = y2;
y2 = temp;

//	swap(x1, x2);
temp = x1;
x1 = x2;
x2 = temp;

//	swap(u1, u2);
float temp1 = u1;
u1 = u2;
u2 = temp1;

//	swap(v1, v2);
temp1 = v1;
v1 = v2;
v2 = temp1;

//	swap(w1, w2);
temp1 = w1;
w1 = w2;
w2 = temp1;
}
//-------------------------------
if (y3 < y1)	{
//		swap(y1, y3);
int temp = y1;
y1 = y3;
y3 = temp;

//		swap(x1, x3);
temp = x1;
x1 = x3;
x3 = temp;

//	swap(u1, u3);
float temp1 = u1;
u1 = u3;
u3 = temp1;

//	swap(v1, v3);
temp1 = v1;
v1 = v3;
v3 = temp1;

//	swap(w1, w3);
temp1 = w1;
w1 = w3;
w3 = temp1;
}

//-------------------------------
if (y3 < y2)	{
//	swap(y2, y3);
int temp = y2;
y2 = y3;
y3 = temp;

//	swap(x2, x3);
temp = x2;
x2 = x3;
x3 = temp;

//	swap(u2, u3);
float temp1 = u2;
u2 = u3;
u3 = temp1;

//		swap(v2, v3);
temp1 = v2;
v2 = v3;
v3 = temp1;

//		swap(w2, w3);
temp1 = w2;
w2 = w3;
w3 = temp1;
}


// Obliczanie jakie przyrosty sa na wspolrzednych trojkata ( dwa ramiona rozpatrujemy) i UV

int dy1 = y2 - y1; // grow on Y of projected triangle on screen
int dx1 = x2 - x1; // grow on X of projected triangle on screen

 // printf ("dy1 dx1: %d, %d\n", dy1, dx1);

float dv1 = v2 - v1; // Length of U i V of texture
float du1 = u2 - u1;
float dw1 = w2 - w1;

//				char buf13[16];char buf14[16];char buf15[16];
//				printf ("dv1 du1: %s, %s\n", ftoa(dv1, buf13)	, ftoa(du1, buf14)	);

int dy2 = y3 - y1;
int dx2 = x3 - x1;

float dv2 = v3 - v1;
float du2 = u3 - u1;
float dw2 = w3 - w1;

float tex_u, tex_v;
float tex_w;

float dax_step = 0  , dbx_step = 0, // scanline step on raster
	  du1_step = 0  , dv1_step = 0,
	  du2_step = 0  , dv2_step = 0,
	  dw1_step = 0	, dw2_step = 0;

if (dy1>0) dax_step = dx1 / (float)abs(dy1);   // calculation of step proportion
if (dy2>0) dbx_step = dx2 / (float)abs(dy2);

if (dy1>0) du1_step = du1 / (float)abs(dy1);
if (dy1>0) dv1_step = dv1 / (float)abs(dy1);
if (dy1>0) dw1_step = dw1 / (float)abs(dy1);

if (dy2>0) du2_step = du2 / (float)abs(dy2);
if (dy2>0) dv2_step = dv2 / (float)abs(dy2);
if (dy2>0) dw2_step = dw2 / (float)abs(dy2);


int penno;

if (dy1>0)


{


	for (int i = y1; i <= y2; i++)



	{
		int ax = x1 + (float)(i - y1) * dax_step;
		int bx = x1 + (float)(i - y1) * dbx_step;



		float tex_su = u1 + (float)(i - y1) * du1_step;
		float tex_sv = v1 + (float)(i - y1) * dv1_step;
		float tex_sw = w1 + (float)(i - y1) * dw1_step;

		float tex_eu = u1 + (float)(i - y1) * du2_step;
		float tex_ev = v1 + (float)(i - y1) * dv2_step;
		float tex_ew = w1 + (float)(i - y1) * dw2_step;



		if (ax > bx)
		{
			// swap(ax, bx);
		  int temp = ax;
			   ax = bx;
			   bx = temp;

			// swap(tex_su, tex_eu);
			   float temp1 = tex_su;
			   tex_su = tex_eu;
			   tex_eu = temp1;


			// swap(tex_sv, tex_ev);
			   temp1 = tex_sv;
			   tex_sv = tex_ev;
			   tex_ev = temp1;

			//swap(tex_sw, tex_ew);
			   temp1 = tex_sw;
			   tex_sw = tex_ew;
			   tex_ew = temp1;

		}



		tex_u = tex_su;
		tex_v = tex_sv;
		tex_w = tex_sw;

		float tstep = 1.0f / ((float)(bx - ax));
		float t = 0.0f;

		for (int j = ax; j < bx; j++)




		{


			penno=ReadPixel( rport, j, i);

							if (penno ==0) {


			tex_u = (((1.0f - t) * tex_su + t * tex_eu)*widthTxt)-0.5f;
			tex_v = (((1.0f - t) * tex_sv + t * tex_ev)*heightTxt)-0.5f; //-2;
			tex_w = ((1.0f - t) * tex_sw + t * tex_ew);


			int textPixel_u= (int)tex_u ;          // calculate texture pixel using proportion of raster to sample colour;
			int textPixel_v= (int)tex_v ;


		 int textPixel_w= (int)tex_w;

		 sampleColour= iffTexture[textPixel_u][textPixel_v];

		 if (tex_w > pDepthBuffer[i*SCR_WIDTH + j])
			{

				sampleColour= iffTexture[textPixel_u/textPixel_w][textPixel_v/textPixel_w];
			}

int pixel_bw=sampleColour;
if(pixel_bw>15) pixel_bw=15;

//arrayScreen [(i * SCR_WIDTH) + j]=pixel_bw;
//
//
SetAPen(rport,pixel_bw);
 WritePixel(rport, j, i);

				pDepthBuffer[(i*SCR_WIDTH) + j] = tex_u;
		//	}

			t += tstep;
		}}

	}

}

dy1 = y3 - y2;
dx1 = x3 - x2;
dv1 = v3 - v2;
du1 = u3 - u2;
dw1 = w3 - w2;

if (dy1) dax_step = dx1 / (float)abs(dy1);
if (dy2) dbx_step = dx2 / (float)abs(dy2);

du1_step = 0, dv1_step = 0;
if (dy1) du1_step = du1 / (float)abs(dy1);
if (dy1) dv1_step = dv1 / (float)abs(dy1);
if (dy1) dw1_step = dw1 / (float)abs(dy1);

if (dy1)
{
	int j;
	for (int i = y2; i <= y3; i++)


	{
		int ax = x2 + (float)(i - y2) * dax_step;
		int bx = x1 + (float)(i - y1) * dbx_step;

		float tex_su = u2 + (float)(i - y2) * du1_step;
		float tex_sv = v2 + (float)(i - y2) * dv1_step;
		float tex_sw = w2 + (float)(i - y2) * dw1_step;

		float tex_eu = u1 + (float)(i - y1) * du2_step;
		float tex_ev = v1 + (float)(i - y1) * dv2_step;
		float tex_ew = w1 + (float)(i - y1) * dw2_step;

		if (ax > bx)
		{

//			swap(ax, bx);
		int temp = ax;
			ax = bx;
			bx = temp;

					// swap(tex_su, tex_eu);
					   float temp1 = tex_su;
					   tex_su = tex_eu;
					   tex_eu = temp1;

					// swap(tex_sv, tex_ev);
					   temp1 = tex_sv;
					   tex_sv = tex_ev;
					   tex_ev = temp1;

					//swap(tex_sw, tex_ew);
					   temp1 = tex_sw;
					   tex_sw = tex_ew;
					   tex_ew = temp1;
		}

		tex_u = tex_su;
		tex_v = tex_sv;
		tex_w = tex_sw;

		float tstep = 1.0f / ((float)(bx - ax));
		float t = 0.0f;


		for (j = ax; j < bx; j++)



		{
			penno=ReadPixel( rport, j, i);

			if (penno ==0) {

			tex_u = (((1.0f - t) * tex_su + t * tex_eu)*widthTxt)-0.5f;
			tex_v = (((1.0f - t) * tex_sv + t * tex_ev)*heightTxt)-0.5f; //-2;
			tex_w = ((1.0f - t) * tex_sw + t * tex_ew);

							int textPixel_u= (int)tex_u ;    // /100;
							int textPixel_v= (int)tex_v ; // /100;
							int textPixel_w= (int)tex_w;


							int sampleColour= iffTexture[textPixel_u][textPixel_v];

							if (tex_w > pDepthBuffer[i*SCR_WIDTH + j])
													{
				 sampleColour= iffTexture[textPixel_u/textPixel_w][textPixel_v/textPixel_w];
		}

				int pixel_bw=sampleColour;
				if(pixel_bw>15) pixel_bw=15;


//  arrayScreen [(i * SCR_WIDTH) + j]=pixel_bw;


	SetAPen(rport,pixel_bw);
	 WritePixel(rport, j, i);

		//		Draw(j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
				pDepthBuffer[i*SCR_WIDTH + j] = tex_w;
	//		}

			t += tstep;
				}}
			}
	}

}
}

							}


						}

//						WritePixelArray8( rport, 0, 0, SCR_WIDTH-1, SCR_HEIGHT-1, arrayScreen, rport );
//						memset(arrayScreen, 0, sizeof arrayScreen); /* Use size of the array itself. */



						// TEXTURE PREVIEW FOR DEBUG
//						for (int i=0; i<64; i++){
//							for(int j=0; j<64; j++){
//
//								pixel_bw=iffTexture[j][i];
//								//
//								//
//								SetAPen(rport,pixel_bw);
//								 WritePixel(rport, 220+j, 180-i);
//
//							}
//
//						}






						frameNo++; // frame counter

						GetSysTime(tv);                    // check current time
						endTime = tv->tv_micro;
						dilatationTime = (endTime - startTime); // time of 1 frame
						if (dilatationTime>0) {
						fps = 1000000 / dilatationTime; // ? frames per second
						}
//* User interface */

						int yInfo=SCR_HEIGHT*0.82, yInfo0 = yInfo+70;

						SetAPen(rport, 1);


				// fps
						Move(rport, 10, yInfo+15);
						Text(rport, "FPS: ", 5);
						itoa(fps, str1, 10);
						Move(rport, 90, yInfo+15);
						Text(rport, str1, strlen(str1));              // fps d

						int xInfo1=SCR_WIDTH-100, xInfo2 = xInfo1+70;
//
				// Slot 1
						Move(rport, xInfo1, 10);
						Text(rport, "Faces: ", 7);
						itoa(nrFaces, slot1, 10);		// x
						Move(rport, xInfo2, 10);
						Text(rport,slot1, strlen(slot1));
//
				// slot 2
						Move(rport, xInfo1, 22);
						Text(rport, "Vertices: ", 10);
						itoa(nrVertices, slot2, 10);		// y
						Move(rport, xInfo2, 22);
						Text(rport,slot2, strlen(slot2));

//				// slot 3
						Move(rport, xInfo1, 36);
						Text(rport, "Screen W: ", 10);
						itoa(SCR_WIDTH, slot3, 10);		// z
						Move(rport, xInfo2, 36);
						Text(rport,slot3, strlen(slot3));

//				// slot 4
						Move(rport, xInfo1, 48);
						Text(rport, "Screen H: ", 10);
						itoa(SCR_HEIGHT, slot4, 10);		// x
						Move(rport, xInfo2, 48);
						Text(rport,slot4, strlen(slot4));

//				// slot 5
						Move(rport, xInfo1, 60);
						Text(rport, "Norm.fc: ", 9);
						itoa(nrNormalFaces, slot5, 10);		// y
						Move(rport, xInfo2, 60);
						Text(rport,slot5, strlen(slot5));


						/* update the physical display to match the newly drawn bitmap. */


						MakeScreen(screen); /* Tell intuition to do its stuff.          */
						RethinkDisplay(); /* Intuition compatible MrgCop & LoadView   */
						/*               it also does a WaitTOF().  */
						/* switch the frame number for next time through */

						startTime = endTime;
						toggleFrame ^= 1;
						nrNormalFaces=0;


					//	CLEAR=1;


					}
					   // printf ("Time stamp  - %d\n", endTime/10000);

					CloseDevice((struct IORequest*) tr);
				}
				FreeMem(clockdata, sizeof(struct ClockData));
			}
			FreeMem(tv, sizeof(struct timeval));
		}
		FreeMem(tr, sizeof(struct timerequest));
	}
	CloseLibrary(UtilityBase);


	}
}

/*
**freeBitMaps(): free up the memory allocated by setupBitMaps().
 */
VOID freeBitMaps(struct BitMap **myBitMaps, LONG depth, LONG width, LONG height) {

freePlanes(myBitMaps[0], depth, width, height);
freePlanes(myBitMaps[1], depth, width, height);

FreeMem(myBitMaps[0], (LONG) sizeof(struct BitMap));
FreeMem(myBitMaps[1], (LONG) sizeof(struct BitMap));
}

/*
 ** setupPlanes(): allocate the bit planes for a screen bit map.
 */
LONG setupPlanes(struct BitMap *bitMap, LONG depth, LONG width, LONG height) {
SHORT plane_num;

for (plane_num = 0; plane_num < depth; plane_num++) {

	bitMap->Planes[plane_num] = (PLANEPTR) AllocRaster(width, height);

	if (bitMap->Planes[plane_num] != NULL)
		BltClear(bitMap->Planes[plane_num], (width/8) * height, 1);
	else {
		freePlanes(bitMap, depth, width, height);
		return (NULL);
	}
}
return (TRUE);
}

/*
 ** freePlanes(): free up the memory allocated by setupPlanes().
 */
VOID freePlanes(struct BitMap *bitMap, LONG depth, LONG width, LONG height) {
SHORT plane_num;

for (plane_num = 0; plane_num < depth; plane_num++) {
	if (bitMap->Planes[plane_num] != NULL)
		FreeRaster(bitMap->Planes[plane_num], width, height);
}
}


// break string for tokens and add to array
void getTokens(char *str, char tokens[3][32]) {
	int nrTok = 0;
	char* token = strtok(str, " ");
	while (token) {
		strcpy(tokens[nrTok++], token);
		token = strtok(NULL, " ");
	}
	return;
}

// break string with texture for tokens and add to array

void getTokensB(char *str, char tokensB[6][32]) {
	int nrTokena = 0;
	char* tokenB = strtok(str, " /");
	while (tokenB) {
		strcpy(tokensB[nrTokena++], tokenB);
		tokenB = strtok(NULL, " /");
	}
	return;
}

// break string with palette for tokens and add to array

void getTokensP(char *str, char tokensP[6][32]) {
	int nrTokena = 0;
	char* tokenP = strtok(str, ",");
	while (tokenP) {
		strcpy(tokensP[nrTokena++], tokenP);
		tokenP = strtok(NULL, ",");
	}
	return;
}

// ------------------------------------------------------------------------------------------------------------------FILE INPUT

int fileRead(void) {
	BPTR fh;
	BPTR fhp;
	char lineRead[2048];
	char lineReadPal[8192];

	UBYTE *buffer;
	UBYTE *buffer1;

	char tempStr[3][32];
	char tempStrUV[2][32];

	UBYTE Char1; // lower byte
	UBYTE Char2; // upper byte

	int pixArrOffset;

// char name1[]="i3trim128.bmp";
//	char name1[]="hull.bmp";
//	char name1[]="icarus.bmp";
	char name1[]="txt64x64.bmp";

	fhp = Open(name1, MODE_OLDFILE);

	    if (fhp) {

	    	while (buffer1=FGets(fhp, lineReadPal, 65536L)) {

				  // extract z headera
				  unsigned char widthRead = lineReadPal[18]; // offset for texture width value 18
				  unsigned char  heightRead = lineReadPal[22]; // offset for texture height value 22

				    widthTxt=widthRead;
					heightTxt=heightRead;

				    pixArrOffset=lineReadPal[10];
				 //   int dlugoscPliku=lineReadPal[2];
				    // offset 46 - liczba kolorow
				    // offset 54 - pocz¹tek tablicy kolorow - 4 bajty... g, r ,b , 0
				    printf ("Szerokosc pix - %d\n", widthTxt);
				    printf ("Wysokosc pix - %d\n", heightTxt);


				    int counter=0; // ilosc kolorow
				    int ti=0;

// NARAZIE LADOWANIE PALETY NIE DZIALA - TRZEBA ZROBIC JAKOS UPDATE PALETY PO STARCIE
for(int i=0; i<heightTxt; i++){
				    for(int j = 0; j < widthTxt*4; j ++)
				            {
				    	unsigned char colorValue = lineReadPal[54+j];
				    			int colVal = colorValue;

				    			// populate palette array
				    			if (ti=0){
				    			palette[counter][0] = counter;
				    			palette[counter][3] = colVal;}

				    			if (ti=1){
				    			palette[counter][2] = colVal;}

				    			if (ti=2){
				    			palette[counter][1] = colVal;}

				            	ti++; if (ti>3) { ti=0 , counter++;};
				            }
}

// transfer texture pixel by pixel to array

	int offsetPixel;
	int pixel1;
	int pixel2;


int progress=0;

// po dwa pixele do tablicy z bajta dlatego dziele widthText/2

for(int j=0; j<heightTxt; j++){

	for (int k=0; k<widthTxt/2; k++){

        if (progress==(widthTxt/2)){ offsetPixel = (offsetPixel+widthTxt/2); progress=0;}

		unsigned char pixelCol = lineReadPal[pixArrOffset+k+offsetPixel-1]; // offset behind the palette
	      			           Char1 = pixelCol & 0x0F;
	      			           Char2 = (pixelCol >> 4);

	      			           pixel1=Char2;
	      			           pixel2=Char1;

	      			           iffTexture[k+progress][j]=pixel1;

	      			           iffTexture[k+1+progress][j]=pixel2;

							    progress++;


	    }
	}
}
	    	Flush( fhp );
Close(fhp);
}



// char name[]="ship2.obj";
char name[]="kostka.obj";
//char name[]="icarus.obj";
//char name[]="i3.obj";
    fh = Open(name, MODE_OLDFILE);

    if (fh) 			{
	//	printf ("File open: %s\n", name);
		while (buffer = FGets(fh, lineRead, 16384L))
							{


			if (lineRead[0] == 'v')
								{

				if (lineRead[1] == 't')       // is texture ?
									{

					hasTexture=1;

						getTokens(&lineRead[3], tempStrUV);
								for (int t=0; t<2; t++)
											{

//
//									int convertUVToInt = atof(tempStrUV[t]);
//
//
//									 texUV[nrUV][t] = convertUVToInt;		// convert UV vertices U(x) V(y) to array float



								 	texUV[nrUV][t] = atof(tempStrUV[t]);		// convert UV vertices U(x) V(y) to array float

//																		char buf1[16];

											}
								texUV[nrUV][2] = wt;
								nrUV++;
										}
				else
										{

				//	printf ("TBrak uv textury w pliku: %s\n", name);

				getTokens(&lineRead[2], tempStr);
				for (int t=0; t<3; t++)		{


					int convertToInt = atof(tempStr[t])*100;

					vertices[nrVertices][t] = convertToInt ;		// convert vertices to float

				//	vertices[nrVertices][t] = atof(tempStr[t])*10;		// convert vertices to float


											}
				vertices[nrVertices][3] = w;
				nrVertices++;
				if (nrVertices>=MAX_VERTICES)
											{
					printf ("ERROR: Max number of vertices exceeded: %d\n", MAX_VERTICES);
					return 1;
											}
										}
									}
			else if (lineRead[0] == 'f')
									{

					if (hasTexture==0)
										{
						// IF NO TEXTURES

										getTokens(&lineRead[2], tempStr);
										for (int t=0; t<3; t++) {
											faces[nrFaces][t] = atoi(tempStr[t]) - 1;			// convert interconections vertices array. Subtract 1 for index satarting of 0.
										}

										nrFaces++;
										if (nrFaces>=MAX_FACES)
										{
											printf ("ERROR: Max number of faces exceeded: %d\n", nrFaces);
											return 1;
										}
									}

					if (hasTexture!=0)
									{     // IF TEXTURE EXISTS break tonen in backslash and add to faces[] and textUV[]

				getTokensB(&lineRead[2], tempStr);

// Vert normals - // convert vertices and UV  to array. Subtract 1 for index satarting of 0.

		faces[nrFaces][0] = atoi(tempStr[0])-1;
		faces[nrFaces][1] = atoi(tempStr[2])-1;
		faces[nrFaces][2] = atoi(tempStr[4])-1;

//  UV normals coords

		faces[nrFaces][3] = atoi(tempStr[1])-1;
		faces[nrFaces][4] = atoi(tempStr[3])-1;
		faces[nrFaces][5] = atoi(tempStr[5])-1;

				nrFaces++;
				//nrUV++;

				if (nrFaces>=MAX_FACES)
										{
					printf ("ERROR: Max number of faces exceeded: %d\n", nrFaces);
					return 1;
										}
									}
								}
							}
		Flush( fh );
		Close(fh);
		return 0;
						}
    else 				{

		printf ("ERROR: could not open file: %s\n", name);
		return 1;
						}
					}

VOID main(int argc, char **argv) {

	struct BitMap **myBitMaps;
	struct Screen *screen;
	struct NewScreen myNewScreen = {0};
//	struct AreaInfo ainfo = {0};
	struct Window *win;

//int iffTexture[64][64]={0};
//
//int vertices[MAX_VERTICES][4] = {0};
//
//int faces[MAX_FACES][6] = {0};
//
//float texUV[MAX_VERTICES][3]={0};

	struct ColorSpec  cols[17] = {		// ColorIndex, R, G, B

					{0,0,0,3},
					{1,15,15,15},  	// white
					{2,13,13,13},		// grey
					{3,9,9,9},   // l grey
					{4,6,6,6}, 	// blue
					{5,3,3,2},
					{6,7,6,4},  	// yellow
					{7,7,7,5},
					{8,8,11,6},   	// navy
					{9,6,9,6},   	// red
					{10,4,12,9},
					{11,4,10,12},
					{12,3,6,8},
					{13,15,15,3},
					{14,13,10,6},
					{15,12,5,5},
					{-1,0,0,0}
		};


	struct TagItem tags[2] = {
		{SA_Colors, (ULONG)cols},
		{TAG_END, 0}
	};


	int retVal;
//	 retVal = fileOpen();
	retVal =fileRead();
//	fileOpen(iffTexture, texUV, vertices, faces);

if (retVal!=0) return;	// quit if error reading mesh

//	Printf("\nPress Enter to continue\n"); getchar();

	IntuitionBase = OpenLibrary("intuition.library", 33L);
	if (IntuitionBase != NULL) {
		GfxBase = OpenLibrary("graphics.library", 33L);
		if (GfxBase != NULL) {


			myBitMaps = setupBitMaps(SCR_DEPTH, SCR_WIDTH, SCR_HEIGHT);
			if (myBitMaps != NULL

			) {
				/* Open a simple quiet screen that is using the first
				 ** of the two bitmaps.
				 */
				myNewScreen.LeftEdge = 0;
				myNewScreen.TopEdge = 0;
				myNewScreen.Width = SCR_WIDTH;
				myNewScreen.Height = SCR_HEIGHT;
				myNewScreen.Depth = SCR_DEPTH;
				myNewScreen.DetailPen = 0;
				myNewScreen.BlockPen = 1;

if( SCR_WIDTH ==320 && SCR_HEIGHT == 200 ) { myNewScreen.ViewModes = 0; };
if( SCR_WIDTH ==320  && SCR_HEIGHT == 400) { myNewScreen.ViewModes = 0 | LACE ;};
if( SCR_WIDTH ==640  && SCR_HEIGHT == 200) { myNewScreen.ViewModes = HIRES;};
if( SCR_WIDTH ==640  && SCR_HEIGHT == 400) { myNewScreen.ViewModes = HIRES | LACE;};
				myNewScreen.Type = CUSTOMSCREEN | CUSTOMBITMAP | SCREENQUIET;
				myNewScreen.Font = NULL;
				myNewScreen.DefaultTitle = NULL;
				myNewScreen.Gadgets = NULL;
				myNewScreen.CustomBitMap = myBitMaps[0];

				screen = OpenScreenTagList(&myNewScreen, tags);

				if (screen != NULL) {



					            win = OpenWindowTags(NULL,
					            						WA_CustomScreen, screen,

														WA_Borderless,   TRUE,
														WA_Backdrop,     FALSE,
														WA_NoCareRefresh, TRUE,
														WA_IDCMP,        IDCMP_RAWKEY,
														WA_Flags, WFLG_SUPER_BITMAP,
														WA_CloseGadget,  TRUE,
														WA_RMBTrap, TRUE,
														TAG_END);



					  		  if ( win != NULL )
					  					                {



					/* Indicate that the rastport is double buffered. */
					screen->RastPort.Flags = DBUFFER;

//					runDBuff(screen, myBitMaps);
					runDBuff(screen, myBitMaps, win);


					                CloseWindow(win);
					                }

					CloseScreen(screen);
				}
				freeBitMaps(myBitMaps, SCR_DEPTH, SCR_WIDTH, SCR_HEIGHT);


			}
			CloseLibrary(GfxBase);
		}
		CloseLibrary(IntuitionBase);
	}

}




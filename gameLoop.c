#include <stdio.h>
#include <stdlib.h>
#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <proto/exec.h>   // operacje io dysk
#include <clib/utility_protos.h>
#include <clib/timer_protos.h>
#include <devices/timer.h>

#include "textureFileOpen.h"
#include "globals.h"
#include "WindowGetSignal.h"
#include "quicksort.h"

int nrVertices=0;

struct Device *TimerBase;
struct Library *UtilityBase;

ULONG windowSignalGame=0;


#define MAX_VERTICES	1000
#define MAX_FACES		1000


#define KEY_ESC 0x45
#define A_RIGHT 0x4e
#define A_LEFT 0x4f
#define A_UP 0x4c
#define A_DN 0x4d
#define KEY_ENTER 0x44
#define KEY_W 0x11
#define KEY_S 0x21
#define KEY_A 0x20
#define KEY_D 0x22

int vertices[MAX_VERTICES][4] = {{0}};
//Vertex *vertices = NULL;
int faces[MAX_FACES][6] = {{0}};
float texUV[MAX_VERTICES][3]={{0}};
int texs[MAX_FACES][2]={{0}};
int nrUV;
int hasTexture = 0;

int palette[16][4]={0};
int iffTexture[64][64]={{0}};
int widthTxt=64;
int heightTxt=64;

int nrFaces = 0;

int CLEAR =1;

int w=1;
int wt=1;

int xClip1;
int yClip1;
int xClip2;
int yClip2;
int xClip3;
int yClip3;

int pDepthBuffer[MAX_VERTICES]={0};

int sampleColour=0;

int number[MAX_FACES]={0}; // tyle co ogolna liczba trojkatow w scenie / max triangles in scene
int faceindex[MAX_FACES]={0}; // tu beda posortowane indeksy fejsow / sorted out triangle indexes
int nrNormalFaces=0;

int X1, X2, X3, Y1, Y2, Y3;


int clipTest=0;
int t1=0;  // test bits
int t2=0;
int t3=0;


int xm, xm2, xm3, ym, ym2 , ym3;

int x1Projected=0;
int y1Projected=0;
int x2Projected=0;
int y2Projected=0;
int x3Projected=0;
int y3Projected=0;


int tMapRefferPoint1 = 0;
int tMapRefferPoint2 = 0;
int tMapRefferPoint3 = 0;

float p1mapU = 0;
float p1mapV = 0;
float p1mapW = 0;

float p2mapU = 0;
float p2mapV = 0;
float p2mapW = 0;

float p3mapU = 0;
float p3mapV = 0;
float p3mapW = 0;

int referPoint1 = 0;
int referPoint2 = 0;
int referPoint3 = 0;
int line1x = 0;
int line1y = 0;
//		int line1z = 0;

int line2x = 0;
int line2y = 0;
//		int line2z = 0;

//		float normalx = 0;
//		float normaly = 0;
int normalz = 0;

int referSort1 = 0;
int referSort2 = 0;
int referSort3 = 0;

int zBuff=0;

int x_orig=0, y_orig=0, z_orig=0, x1B=0, x2B=0, x3B=0, y1B=0, y2B=0, y3B=0, z1B=0, z2B=0, z3B=0;
int u_orig=0, v_orig=0, w_orig=0, x1t=0, x2t=0, x3t=0, y1t=0, y2t=0, y3t=0, z1t=0, z2t=0, z3t=0;
int temp=0;
float temp1=0;

int dy1=0;
int dx1=0;
float dv1 =0;
float du1 =0;
float dw1 =0;

int dy2 =0;
int dx2 =0;

float dv2 =0;
float du2 =0;
float dw2 =0;

float tex_u, tex_v;
float tex_w;

float dax_step = 0  , dbx_step = 0,
	  du1_step = 0  , dv1_step = 0,
	  du2_step = 0  , dv2_step = 0,
	  dw1_step = 0	, dw2_step = 0;

int penno=0;

int ax =0;
		int bx =0;

float tex_su =0;
float tex_sv =0;
float tex_sw =0;

float tex_eu =0;
float tex_ev =0;
float tex_ew =0;
float tstep = 0;
float t = 0;

int textPixel_u= 0;
int textPixel_v= 0;
int textPixel_w= 0;

int result[MAX_VERTICES][6] = {{0}};
int pixel_bw=0;


int frameNo = 0;

int fps=0;

struct ClockData *clockdata;
struct timerequest *tr;
struct timeval *tv;

LONG dilatationTime;
LONG startTime;
LONG endTime;

WORD toggleFrame=0;


int addX=0;
int addZ=0;

//rotacja
int rotZ=0;
int rotY=0;
int rotX=0;



// ------------------------------------------------------------------------------------------------

// print float to provided string buffer

char * ftoa(float f, char *b) {
	sprintf(b, "%s%d.%d",(f<0)?"-":"",abs((int)f),abs((int)((f - (int)f)*1000000)));
	return b;
}

// pre-calculated sin and cos implementation

float sintab[91] = {0.000000,0.017452,0.034899,0.052336,0.069756,0.087156,0.104528,0.121869,0.139173,0.156434,0.173648,0.190809,
		0.207912,0.224951,0.241922,0.258819,0.275637,0.292372,0.309017,0.325568,0.342020,0.358368,0.374607,0.390731,0.406737,
		0.422618,0.438371,0.453990,0.469472,0.484810,0.500000,0.515038,0.529919,0.544639,0.559193,0.573576,0.587785,0.601815,
		0.615661,0.629320,0.642788,0.656059,0.669131,0.681998,0.694658,0.707107,0.719340,0.731354,0.743145,0.754710,0.766044,
		0.777146,0.788011,0.798636,0.809017,0.819152,0.829038,0.838671,0.848048,0.857167,0.866025,0.874620,0.882948,0.891007,
		0.898794,0.906308,0.913545,0.920505,0.927184,0.933580,0.939693,0.945519,0.951057,0.956305,0.961262,0.965926,0.970296,
		0.974370,0.978148,0.981627,0.984808,0.987688,0.990268,0.992546,0.994522,0.996195,0.997564,0.998630,0.999391,0.999848,1.0};

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
void gameLoop(struct RastPort *rp, struct Screen *screen, struct BitMap **myBitMaps, struct Window *win){

int scr_width =win->MaxWidth;	// SET RESOLUTION - 320x200; 320x400; 640x200; 640x400
int scr_height=win->MaxHeight;


	int yInfo=scr_height*0.82;
//	int yInfo0 = yInfo+70;
	int xInfo1=scr_width-100;
	int xInfo2 = xInfo1+70;

fileRead();


// Projection Matrix
//	float fNear = 0.1f;

float fFar = 100000.0f;    // od tego zalezy ogniskowa dla wierzcholkow w int  (1000 dla float)
float fFov = 150.0f;
float fAspectRatio = 1.5f;

//	float fFovRad = 1.0f;

float modifierX=scr_width*0.0005f;								// 0.2 -> 320 width
float modifierY=scr_height*0.0013f ;

//float modifierX=0.16f;								// 0.2 -> 320 width
//float modifierY=0.26f ;

//float correction = 0.39f;
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


					char str1[123];

					char slot1[123], slot2[123], slot3[123], slot4[123], slot5[123];

					int angles[3][2] = {{0,0}, {0,0}, {0,0}};  // for all planes: { pozioma x, pionowa y, pozioma rownolegla do ekranu Z  - current angle, rotation speed }
					int angle;



//----------------------------------------------------------------------LOOP FOR ktr = number of frames-----------------------------------------------------------------------------------------------------
					//for (int ktr = 0; ktr <50; ktr++) {
						for (;;) {


//								windowSignalGame=WindowGetSignal(win);
//									    if (windowSignalGame == KEY_ESC) { break; }



						/* switch the bitmap so that we are drawing into the correct place */
							    screen->RastPort.BitMap          = myBitMaps[toggleFrame];
								screen->ViewPort.RasInfo->BitMap = myBitMaps[toggleFrame];
//									    SetAPen(rp, 1);
									    SetRast(rp, 0);

						//------------------------------------ najpierw obroc wszystkie trojkaty--------------------------- //

						for (int k=0; k<nrVertices; k++) {

							// odczytaj po jednym wierzcholku / read vertice by vertice

								x_orig = vertices[k][0];
								y_orig = vertices[k][1];
								z_orig = vertices[k][2];

								u_orig = texUV[k][0];
								v_orig = texUV[k][1];
								w_orig = texUV[k][2];

//								 if (windowSignalGame == A_LEFT){x_orig++;}

// usunac potem: przesun bryle tak, zeby srodek obrotu byl w srodku a nie w jednym rogu / it is for moving axis of rotation

//x_orig -=150; // Lokalizacja kamery / camera usable
// y_orig +=50;
// z_orig +=150;
//								if (windowSignalGame == A_RIGHT){rotY--; angles[1][1]=rotY;}
//								if (windowSignalGame == A_LEFT){rotY++; angles[1][1]=rotY;}
//								if (windowSignalGame == A_DN){rotZ--; angles[2][1]=rotZ;}
//								if (windowSignalGame == A_UP){rotZ++; angles[2][1]=rotZ;}

								angle = angles[0][0];
								x1B = x_orig*kcos(angle) - y_orig*ksin(angle);
								y1B = x_orig*ksin(angle) + y_orig*kcos(angle);
								z1B = z_orig;



								x1t = u_orig*kcos(angle) - v_orig*ksin(angle);	// texture related
								y1t = v_orig*ksin(angle) + v_orig*kcos(angle);
								z1t = w_orig;

								angle = angles[1][0];
								x2B = x1B*kcos(angle) - z1B*ksin(angle);
								y2B = y1B;
								z2B = x1B*ksin(angle) + z1B*kcos(angle);

								x2t = x1t*kcos(angle) - z1t*ksin(angle);
								y2t = y1t;
								z2t = x1t*ksin(angle) + z1t*kcos(angle);


								angle = angles[2][0];
								x3B = x2B;
								y3B = y2B*kcos(angle) - z2B*ksin(angle);
								z3B = y2B*ksin(angle) + z2B*kcos(angle);

								angle = angles[1][0];
								x3t = x2t;
								y3t = y2t*kcos(angle) - z2t*ksin(angle);
								z3t = y2t*ksin(angle) + z2t*kcos(angle);

								// perspektywa - nieco uproszczona / simplified perspective

								x3B = x3B*(fFar-z3B*fFov)/fFar;
								y3B = y3B*(fFar-z3B*fFov)/fFar;

								x3t = x3t*(fFar-z3t*fFov)/fFar;	// texel
								y3t = y3t*(fFar-z3t*fFov)/fFar;

								// wysrodkuj na ekranie i uwzglednij aspekt ratio i zapamietaj odwrocone trojkaty

								result[k][0] = x3B;
								result[k][1] = y3B;
								result[k][2] = z3B;

								result[k][3] = x3t;
								result[k][4] = y3t;
								result[k][5] = z3t;



						}




						// zupdatuj wszystkie katy obrotu / update all angles
						for (int t=0; t<3; t++) {

							angles[t][0] += angles[t][1];
						}

// QUICK SORTING - sorting of all polygons over Z


for (int e = 0 ; e < nrFaces; e++){

												    		  referSort1 = faces[e][0];
												    		  referSort2 = faces[e][1];
												    		  referSort3 = faces[e][2];

												    		 zBuff = (result[referSort1][2] + result[referSort2][2] + result[referSort3][2]) / 3.0f;

												    		 zBuff*=10;

												    		number[e]=  (int)zBuff;
															faceindex[e] = e;
									 }

quicksort(number, faceindex, 0, nrFaces-1);

// Draw every triangle of the array / narysuj wszystkie trojkaty z tablicy


						for (int k=0; k<nrFaces; k++) {

							 // Normalise polygons

							 referPoint1 = faces[faceindex[k]][0];		//Ver1 referPoint to pozycja porzadkowa wierzcholka na liscie faces ktora przechowuje x, y, z od poligonu o indeksie k
							 referPoint2 = faces[faceindex[k]][1];		//Ver2
							 referPoint3 = faces[faceindex[k]][2];		//Ver3

							 line1x = result[referPoint2][0] - result[referPoint1][0];
							 line1y = result[referPoint2][1] - result[referPoint1][1];
					//		int line1z = result[referPoint2][2] - result[referPoint1][2];

							 line2x = result[referPoint3][0] - result[referPoint1][0];
							 line2y = result[referPoint3][1] - result[referPoint1][1];
					//		int line2z = result[referPoint3][2] - result[referPoint1][2];

							// Dot product
					//		float normalx = line1y * line2z - line1z * line2y;
					//		float normaly = line1z * line2x - line1x * line2z;
							 normalz = line1x * line2y - line1y * line2x;


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

								tMapRefferPoint1 = faces[faceindex[k]][3]; // pointing index of vertice
								tMapRefferPoint2 = faces[faceindex[k]][4];
								tMapRefferPoint3 = faces[faceindex[k]][5];

								 p1mapU = texUV[tMapRefferPoint1][0];
								 p1mapV = texUV[tMapRefferPoint1][1];
								 p1mapW = texUV[tMapRefferPoint1][2];

								 p2mapU = texUV[tMapRefferPoint2][0];
								 p2mapV = texUV[tMapRefferPoint2][1];
								 p2mapW = texUV[tMapRefferPoint2][2];

								 p3mapU = texUV[tMapRefferPoint3][0];
								 p3mapV = texUV[tMapRefferPoint3][1];
								 p3mapW = texUV[tMapRefferPoint3][2];




							//	SetOutlinePen(rp, pixel_bw-4);    					// FOR EDGES
				SetAPen(rp,pixel_bw);
				// KOREKTA EKRANOWA
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

								x1Projected=(result[referPoint1][0] +400 )*modifierX;								// 0.2 -> 320 width
								y1Projected=(result[referPoint1][1] +300 )/fAspectRatio * modifierY ;				//0.3 -> 200 width
								x2Projected=(result[referPoint2][0] +400 )*modifierX;
								y2Projected=(result[referPoint2][1] +300 )/fAspectRatio * modifierY;
								x3Projected=(result[referPoint3][0] +400 ) *modifierX;
								y3Projected=(result[referPoint3][1] +300 )/fAspectRatio * modifierY;

//=================================================================================HERE STARTS DRAWING / TU ZACZYNA SIE RYSOWANIE==================================================================================

								//	CLIPPING Cohen–Sutherland algorithm ???
								//   slope = (y1 - y0) / (x1 - x0)
								//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
								//   y = y0 + slope * (xm - x0), where xm is xmin or xmax


								// CASE TEST - ONLY FILLED VECTORS AT THE MOMENT

									if ( x1Projected<0 || x1Projected>=scr_width || y1Projected<0 || y1Projected>=scr_height){ t1=1;	} else { t1=0;}

									if ( x2Projected<0 || x2Projected>=scr_width || y2Projected<0 || y2Projected>=scr_height){ t2=1;	} else { t2=0;}

									if ( x3Projected<0 || x3Projected>=scr_width || y3Projected<0 || y3Projected>=scr_height){ t3=1;	} else { t3=0;}

									clipTest= t1+t2+t3;

								//==============================================clipping============================================================================================================
									X1= x1Projected; X2=x2Projected; X3=x3Projected; Y1=y1Projected; Y2= y2Projected; Y3=y3Projected;



// case 1 - TYLKO JEDEN PUNKT ZA EKRANEM

if ( clipTest==1 ) {

									if( y1Projected<0 || y1Projected>=scr_height ){

										if (y1Projected<0 ) {ym=0;} else ym= scr_height-1;
								xClip1 = X1 + (X2 - X1) * (ym - Y1) / (Y2 - Y1);   // 0 - minimalna ekrany
								if (y1Projected>=scr_height-1) {yClip1=scr_height-1;} else { yClip1 = 0;}


								xClip2 = X1 + (X3 - X1) * (ym - Y1) / (Y3 - Y1);
								if (y1Projected>=scr_height-1) {yClip2=scr_height-1;} else { yClip2 = 0;}



											AreaMove(rp, xClip1, yClip1);				// NEW vert 1
											AreaDraw(rp, x2Projected, y2Projected);
											AreaDraw(rp, x3Projected, y3Projected);
											AreaEnd(rp);

											AreaMove(rp, xClip1, yClip1);				// NEW vert 2
											AreaDraw(rp, x3Projected, y3Projected);
											AreaDraw(rp, xClip2,yClip2);
											AreaEnd(rp);

										//	printf ("P1 Y OUT %d\n", y1Projected);

}

								else
									if( y2Projected<0 || y2Projected>=scr_height ){
										if (y2Projected<0 ) {ym=0;} else ym= scr_height-1;
											xClip1 = X2 + (X1 - X2) * (ym - Y2) / (Y1 - Y2);   // 0 - minimalna ekrany

											if (y2Projected>=scr_height) {yClip1=scr_height-1;} else { yClip1 = 0;}
											xClip2 = X2 + (X3 - X2) * (ym - Y2) / (Y3 - Y2);
											if (y2Projected>=scr_height) {yClip2=scr_height-1;} else { yClip2 = 0;}

// Rysowanie wektorow
											AreaMove(rp, x1Projected,y1Projected);				// NEW vert 1
											AreaDraw(rp, xClip1, yClip1);
											AreaDraw(rp, xClip2,yClip2);
											AreaEnd(rp);

											AreaMove(rp, x1Projected, y1Projected);				// NEW vert 2
											AreaDraw(rp,  xClip2,yClip2);
											AreaDraw(rp, x3Projected,y3Projected);
											AreaEnd(rp);

									//printf ("P2 Y OUT %d\n", y2Projected);

									}
											else

									if( y3Projected<0 || y3Projected>=scr_height ){
									if (y3Projected<0 ) {ym=0;} else ym= scr_height-1;
									xClip1 = X3 + (X1 - X3) * (ym - Y3) / (Y1 - Y3);   // 0 - minimalna ekrany
									if (y3Projected>=scr_height) {yClip1=scr_height-1;} else { yClip1 = 0;}
									xClip2 = X3 + (X2 - X3) * (ym - Y3) / (Y2 - Y3);
									if (y3Projected>=scr_height) {yClip2=scr_height-1;} else { yClip2 = 0;}


									AreaMove(rp, x1Projected,y1Projected);				// NEW vert 1
									AreaDraw(rp, xClip1, yClip1);
									AreaDraw(rp, xClip2,yClip2);
									AreaEnd(rp);

									AreaMove(rp, x1Projected, y1Projected);				// NEW vert 2
									AreaDraw(rp, x2Projected, y2Projected);
									AreaDraw(rp, xClip2, yClip2);
									AreaEnd(rp);

									//printf ("P3 Y OUT %d\n", y3Projected);

}
									else

									if( x1Projected<0 || x1Projected>=scr_width ){

									if (x1Projected<0 ) {xm=0;} else xm= scr_width-1;

									yClip1 = Y1 + (Y2 - Y1) * (xm - X1) / (X2 - X1);   // 0 - minimalna ekrany
									yClip2 = Y1 + (Y3 - Y1) * (xm - X1) / (X3 - X1);

									if (x1Projected>=scr_width) {xClip1=scr_width-1; xClip2=scr_width-1;} else { xClip1 = 0; xClip2 = 0;}


									AreaMove(rp, xClip1, yClip1);				// NEW vert 1
									AreaDraw(rp, x2Projected, y2Projected);
									AreaDraw(rp, x3Projected, y3Projected);
									AreaEnd(rp);
									AreaMove(rp, xClip1, yClip1);				// NEW vert 2
									AreaDraw(rp, x3Projected, y3Projected);
									AreaDraw(rp, xClip2, yClip2);
									AreaEnd(rp);

									//printf ("P1 X OUT %d\n", x1Projected);

																			}

									else
									if( x2Projected<0 || x2Projected>=scr_width ){

										if (x2Projected<0 ) {xm=0;} else xm= scr_width-1;



									yClip1 = Y2 + (Y1 - Y2) * (xm - X2) / (X1 - X2);   // 0 - minimalna ekrany
									if (x2Projected>=scr_width) {xClip1=scr_width-1;} else { xClip1 = 0;}
									yClip2 = Y2 + (Y3 - Y2) * (xm - X2) / (X3 - X2);
									if (x2Projected>=scr_width) {xClip2=scr_width-1;} else { xClip2 = 0;}

									AreaMove(rp, x1Projected,y1Projected);				// NEW vert 1
									AreaDraw(rp, xClip1, yClip1);
									AreaDraw(rp, xClip2,yClip2);
									AreaEnd(rp);

									AreaMove(rp, x1Projected, y1Projected);				// NEW vert 2
									AreaDraw(rp,  xClip2,yClip2);
									AreaDraw(rp, x3Projected,y3Projected);
									AreaEnd(rp);

							        //printf ("P2 X OUT %d\n", x2Projected);

								}

									else
									if( x3Projected<0 || x3Projected>=scr_width ){

									if (x3Projected<0 ) {xm=0;} else xm= scr_width-1;

									yClip1 = Y3 + (Y1 - Y3) * (xm - X3) / (X1 - X3);   // 0 - minimalna ekrany
									if (x3Projected>=scr_width) {xClip1=scr_width-1;} else { xClip1 = 0;}
									yClip2 = Y3 + (Y2 - Y3) * (xm - X3) / (X2 - X3);
									if (x3Projected>=scr_width) {xClip2=scr_width-1;} else { xClip2 = 0;}

									AreaMove(rp, x1Projected,y1Projected);				// NEW vert 1
									AreaDraw(rp, xClip1, yClip1);
									AreaDraw(rp, xClip2,yClip2);
									AreaEnd(rp);

									AreaMove(rp, x1Projected, y1Projected);				// NEW vert 2
									AreaDraw(rp,   xClip2,yClip2);
									AreaDraw(rp,  x2Projected, y2Projected);
									AreaEnd(rp);

								//	printf ("P3 X OUT %d\n", x3Projected);

								}

}


// case 2 - 2 POINTS OFF THE SCREEN ---------------------------------------------------------------

if (clipTest==2) {
	// printf ("Podwojny %d\n", xClip2);

	    if (y1Projected<=0 ) {ym=0;} else  {ym= scr_height-1;}
	    if (x1Projected<=0 ) {xm=0;} else  {xm= scr_width-1;}

		if (y3Projected<=0 ) {ym3=0;} else {ym3= scr_height-1;}
		if (x3Projected<=0 ) {xm3=0;} else {xm3= scr_width-1;}

		if (y2Projected<=0 ) {ym2=0;} else {ym2= scr_height-1;}
		if (x2Projected<=0 ) {xm2=0;} else {xm2= scr_width-1;}

// P1 w ekranie

if ( x1Projected>=0 && x1Projected<=scr_width-1 && y1Projected>=0 && y1Projected<=scr_height-1 && ( y2Projected<0 || y2Projected>=scr_height || x2Projected<0 || x2Projected>=scr_width ) && (y3Projected<0 || y3Projected>=scr_height  || x3Projected<0 || x3Projected>=scr_width)){

	if ( x2Projected<0) { xClip1 = 0;} else if ( x2Projected>=scr_width-1) { xClip1 = scr_width-1; } else {xClip1 = X1 + (X2 - X1) * (ym2 - Y1) / (Y2 - Y1);}   // 0 - minimalna ekrany
	if ( x3Projected<0) { xClip2 = 0;} else if ( x3Projected>=scr_width-1) { xClip2 = scr_width-1; } else {xClip2 = X1 + (X3 - X1) * (ym3 - Y1) / (Y3 - Y1);}
	if ( y2Projected<0) {yClip1 = 0;} else if ( y2Projected>=scr_height-1) { yClip1 = scr_height-1; } else {yClip1 = Y1 + (Y2 - Y1) * (xm2 - X1) / (X2 - X1); }  // 0 - minimalna ekrany
	if ( y3Projected<0) {yClip2 = 0;} else if ( y3Projected>=scr_height-1) { yClip1 = scr_height-1; } else {yClip2 = Y1 + (Y3 - Y1) * (xm3 - X1) / (X3 - X1);}

	AreaMove(rp, x1Projected, y1Projected);				// NEW vert 1
	AreaDraw(rp, xClip1, yClip1);
	AreaDraw(rp, xClip2, yClip2);
	AreaEnd(rp);

}

// P2 w ekranie

else
if ( x2Projected>=0 && x2Projected<=scr_height-1 && y2Projected>=0 && y2Projected<=scr_width-1 &&
		( y1Projected<0 || y1Projected>=scr_height || x1Projected<0 || x1Projected>=scr_width ) && (y3Projected<0 || y3Projected>=scr_height  || x3Projected<0 || x3Projected>=scr_width) ){


	if ( x1Projected<0) { xClip1 = 0;} else if ( x1Projected>=scr_width-1) { xClip1 = scr_width-1; } else {xClip1 = X2 + (X1 - X2) * (ym - Y2) / (Y1 - Y2);}   // 0 - minimalna ekrany
	if ( x3Projected<0) { xClip2 = 0;} else if ( x3Projected>=scr_width-1) { xClip1 = scr_width-1; } else {xClip2 = X2 + (X3 - X2) * (ym3 - Y2) / (Y3 - Y2);}
	if ( y1Projected<0) {yClip1 = 0;} else if ( y1Projected>=scr_height-1) { yClip1 = scr_height-1; } else {yClip1 = Y2 + (Y1 - Y2) * (xm - X2) / (X1 - X2);}   // 0 - minimalna ekrany
	if ( y3Projected<0) {yClip2 = 0;} else if ( y3Projected>=scr_height-1) { yClip1 = scr_height-1; }else {yClip2 = Y2 + (Y3 - Y2) * (xm3 - X2) / (X3 - X2);}


		AreaMove(rp, xClip1, yClip1);				// NEW vert 1
		AreaDraw(rp, x2Projected, y2Projected);
		AreaDraw(rp, xClip2, yClip2);
		AreaEnd(rp);

	//	printf ("P1 and P3 OUT %d\n", x1Projected);

}

// P3 w ekranie

else
if ( x3Projected>=0 && x3Projected<=scr_height-1 && y3Projected>=0 && y3Projected<=scr_width-1 &&
		( y1Projected<0 || y1Projected>=scr_height || x1Projected<0 || x1Projected>=scr_width ) && (y2Projected<0 || y2Projected>=scr_height  || x2Projected<0 || x2Projected>=scr_width) ){


	if ( x1Projected<0) { xClip1 = 0;} else if ( x1Projected>=scr_width-1) { xClip1 = scr_width-1; } else {xClip1 = X3 + (X1 - X3) * (ym - Y3) / (Y1 - Y3);}   // 0 - minimalna ekrany
	if (x2Projected<0) { xClip2 = 0;} else if ( x2Projected>=scr_width-1) { xClip1 = scr_width-1; } else {xClip2 = X3 + (X2 - X3) * (ym2 - Y3) / (Y2 - Y3);}
	if ( y1Projected<0) {yClip1 = 0;} else if ( y1Projected>=scr_height-1) { yClip1 = scr_height-1; } else {	yClip1 = Y3 + (Y1 - Y3) * (xm - X3) / (X1 - X3);}   // 0 - minimalna ekrany
	if ( y2Projected<0) {yClip2 = 0;} else if ( y2Projected>=scr_height-1) { yClip1 = scr_height-1; } else {	yClip2 = Y3 + (Y2 - Y3) * (xm2 - X3) / (X2 - X3);}


		AreaMove(rp, xClip1, yClip1);				// NEW vert 1
		AreaDraw(rp, xClip2, yClip2);
		AreaDraw(rp, x3Projected, y3Projected);
		AreaEnd(rp);

//		printf ("P1 and P2 OUT %d\n", x3Projected);
}

// Wszystko poza ekranem
if (clipTest==3) {

}


}


// CASE 3 wszystko na ekranie
if (clipTest<1) {

{
//AreaMove(rp, x1Projected, y1Projected);
//AreaDraw(rp, x2Projected, y2Projected);
//AreaDraw(rp, x3Projected, y3Projected);
//AreaEnd(rp);



//															// TEXTURE PREVIEW FOR DEBUG
//															for (int i=0; i<64; i++){
//																for(int j=0; j<64; j++){
//
//																	pixel_bw=iffTexture[j][i];
//																	//
//																	//
//																	SetAPen(rp,pixel_bw);
//																	 WritePixel(rp, 220+j, 180-i);
//
//																}
//
//															}

// TEXTUROWANIE

int x1=x1Projected, x2=x2Projected, x3=x3Projected, y1=y1Projected, y2=y2Projected, y3=y3Projected;
float u1=p1mapU, u2=p2mapU, u3=p3mapU, v1=p1mapV, v2=p2mapV, v3=p3mapV, w1=p1mapW, w2=p2mapW, w3=p3mapW;


// Sprawdzanie wspolrzednych ekranowych i ich odwracanie jesli trzeba y2 i y3 musza byc mniejsze od y1 i odpowiednio y2
// Checking screen coords and swapping if needed. y2 and y3 have to be smaller then y1 and y2

if (y2 < y1)	{
//	swap(y1, y2);
temp = y1;
y1 = y2;
y2 = temp;

//	swap(x1, x2);
temp = x1;
x1 = x2;
x2 = temp;

//	swap(u1, u2);
temp1 = u1;
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
temp = y1;
y1 = y3;
y3 = temp;

//		swap(x1, x3);
temp = x1;
x1 = x3;
x3 = temp;

//	swap(u1, u3);
temp1 = u1;
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
temp = y2;
y2 = y3;
y3 = temp;

//	swap(x2, x3);
temp = x2;
x2 = x3;
x3 = temp;

//	swap(u2, u3);
temp1 = u2;
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

dy1 = y2 - y1; // grow on Y of projected triangle on screen
dx1 = x2 - x1; // grow on X of projected triangle on screen

 // printf ("dy1 dx1: %d, %d\n", dy1, dx1);

dv1 = v2 - v1; // Length of U i V of texture
du1 = u2 - u1;
dw1 = w2 - w1;

//				char buf13[16];char buf14[16];char buf15[16];
//				printf ("dv1 du1: %s, %s\n", ftoa(dv1, buf13)	, ftoa(du1, buf14)	);

dy2 = y3 - y1;
dx2 = x3 - x1;

dv2 = v3 - v1;
du2 = u3 - u1;
dw2 = w3 - w1;


if (dy1>0) dax_step = dx1 / (float)abs(dy1);   // calculation of step proportion
if (dy2>0) dbx_step = dx2 / (float)abs(dy2);

if (dy1>0) du1_step = du1 / (float)abs(dy1);
if (dy1>0) dv1_step = dv1 / (float)abs(dy1);
if (dy1>0) dw1_step = dw1 / (float)abs(dy1);

if (dy2>0) du2_step = du2 / (float)abs(dy2);
if (dy2>0) dv2_step = dv2 / (float)abs(dy2);
if (dy2>0) dw2_step = dw2 / (float)abs(dy2);


if (dy1>0)


{

	for (int i = y1; i <= y2; i++)

	{
		ax = x1 + (float)(i - y1) * dax_step;
		bx = x1 + (float)(i - y1) * dbx_step;



		 tex_su = u1 + (float)(i - y1) * du1_step;
		 tex_sv = v1 + (float)(i - y1) * dv1_step;
		 tex_sw = w1 + (float)(i - y1) * dw1_step;

		 tex_eu = u1 + (float)(i - y1) * du2_step;
		 tex_ev = v1 + (float)(i - y1) * dv2_step;
		 tex_ew = w1 + (float)(i - y1) * dw2_step;



		if (ax > bx)
		{
			// swap(ax, bx);
		  temp = ax;
			   ax = bx;
			   bx = temp;

			// swap(tex_su, tex_eu);
			   temp1 = tex_su;
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

		 tstep = 1.0f / ((float)(bx - ax));
		 t = 0.0f;

		for (int j = ax; j < bx; j++)


		{


			penno=ReadPixel(rp, j, i);

							if (penno ==0) {


			tex_u = (((1.0f - t) * tex_su + t * tex_eu)*widthTxt)-0.5f;
			tex_v = (((1.0f - t) * tex_sv + t * tex_ev)*heightTxt)-0.5f; //-2;
			tex_w = ((1.0f - t) * tex_sw + t * tex_ew);


			textPixel_u= (int)tex_u ;          // calculate texture pixel using proportion of raster to sample colour;
			textPixel_v= (int)tex_v ;


		 textPixel_w= (int)tex_w;

		 sampleColour= iffTexture[textPixel_u][textPixel_v];

		 if (tex_w > pDepthBuffer[i*scr_width + j])
			{

				sampleColour= iffTexture[textPixel_u/textPixel_w][textPixel_v/textPixel_w];
			}

pixel_bw=sampleColour;
if(pixel_bw>15) pixel_bw=15;


SetAPen(rp,pixel_bw);
 WritePixel(rp, j, i);
//	printf ("samplecolor: %d, \n", sampleColour);
				pDepthBuffer[(i*scr_width) + j] = tex_u;
		//	}

			t += tstep;
			}
		}

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

	for (int i = y2; i <= y3; i++)


	{
		ax = x2 + (float)(i - y2) * dax_step;
		bx = x1 + (float)(i - y1) * dbx_step;

		tex_su = u2 + (float)(i - y2) * du1_step;
		tex_sv = v2 + (float)(i - y2) * dv1_step;
		tex_sw = w2 + (float)(i - y2) * dw1_step;

		tex_eu = u1 + (float)(i - y1) * du2_step;
		tex_ev = v1 + (float)(i - y1) * dv2_step;
		tex_ew = w1 + (float)(i - y1) * dw2_step;

		if (ax > bx)
		{

//			swap(ax, bx);
		    temp = ax;
			ax = bx;
			bx = temp;

					// swap(tex_su, tex_eu);
					   temp1 = tex_su;
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

		tstep = 1.0f / ((float)(bx - ax));
		t = 0.0f;


		for (int j = ax; j < bx; j++)



		{
			penno=ReadPixel( rp, j, i);

			if (penno ==0) {

			tex_u = (((1.0f - t) * tex_su + t * tex_eu)*widthTxt)-0.5f;
			tex_v = (((1.0f - t) * tex_sv + t * tex_ev)*heightTxt)-0.5f; //-2;
			tex_w = ((1.0f - t) * tex_sw + t * tex_ew);

							textPixel_u= (int)tex_u ;    // /100;
							textPixel_v= (int)tex_v ; // /100;
							textPixel_w= (int)tex_w;


							int sampleColour=iffTexture[textPixel_u][textPixel_v];



							if (tex_w > pDepthBuffer[i*scr_width + j])
													{
				 sampleColour= iffTexture[textPixel_u/textPixel_w][textPixel_v/textPixel_w];
		}

				pixel_bw=sampleColour;
				if(pixel_bw>15) pixel_bw=15;



	SetAPen(rp,pixel_bw);
	WritePixel(rp, j, i);


		//		Draw(j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
				pDepthBuffer[i*scr_width + j] = tex_w;
	//		}

			t += tstep;
							}
						}
					}
				}
			}
		}

	}
}


						frameNo++; // frame counter

						GetSysTime(tv);                    // check current time
						endTime = tv->tv_micro;
						dilatationTime = (endTime - startTime); // time of 1 frame
						if (dilatationTime>0) {
						fps = 1000000 / dilatationTime; // ? frames per second
						}
//* User interface */



						SetAPen(rp, 1);


				// fps
						Move(rp, 10, yInfo+15);
						Text(rp, "FPS: ", 5);
						itoa(fps, str1, 10);
						Move(rp, 90, yInfo+15);
						Text(rp, str1, strlen(str1));              // fps d


//
				// Slot 1
						Move(rp, xInfo1, 10);
						Text(rp, "Faces: ", 7);
						itoa(nrFaces, slot1, 10);		// x
						Move(rp, xInfo2, 10);
						Text(rp,slot1, strlen(slot1));
//
				// slot 2
						Move(rp, xInfo1, 22);
						Text(rp, "Vertices: ", 10);
						itoa(nrVertices, slot2, 10);		// y
						Move(rp, xInfo2, 22);
						Text(rp,slot2, strlen(slot2));

//				// slot 3
						Move(rp, xInfo1, 36);
						Text(rp, "Screen W: ", 10);
						itoa(scr_width, slot3, 10);		// z
						Move(rp, xInfo2, 36);
						Text(rp,slot3, strlen(slot3));

//				// slot 4
						Move(rp, xInfo1, 48);
						Text(rp, "Screen H: ", 10);
						itoa(scr_height, slot4, 10);		// x
						Move(rp, xInfo2, 48);
						Text(rp,slot4, strlen(slot4));

//				// slot 5
						Move(rp, xInfo1, 60);
						Text(rp, "Norm.fc: ", 9);
						itoa(nrNormalFaces, slot5, 10);		// y
						Move(rp, xInfo2, 60);
						Text(rp,slot5, strlen(slot5));


						/* update the physical display to match the newly drawn bitmap. */

//														// TEXTURE PREVIEW FOR DEBUG
//														for (int i=0; i<64; i++){
//															for(int j=0; j<64; j++){
//
//																pixel_bw=iffTexture[j][i];
//																//
//																//
//																SetAPen(rp,pixel_bw);
//																 WritePixel(rp, 220+j, 180-i);
//
//															}
//
//														}

						MakeScreen(screen); /* Tell intuition to do its stuff.          */
						RethinkDisplay(); /* Intuition compatible MrgCop & LoadView   */
						/*               it also does a WaitTOF().  */
						/* switch the frame number for next time through */

						startTime = endTime;
						toggleFrame ^= 1;
						nrNormalFaces=0;


						CLEAR=1;

						windowSignalGame=WindowGetSignal(win);

						if (windowSignalGame == A_RIGHT){rotY--; angles[1][1]=rotY;}
						if (windowSignalGame == A_LEFT){rotY++; angles[1][1]=rotY;}
						if (windowSignalGame == A_DN){rotZ--; angles[2][1]=rotZ;}
						if (windowSignalGame == A_UP){rotZ++; angles[2][1]=rotZ;}


						if (windowSignalGame == KEY_ESC) { break; }


					}
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

	return;
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

// ------------------------------------------------------------------------------------------------------------------FILE INPUT

int fileRead(void) {
 textureFileOpen();

	char tempStr[3][32]={0};
	char tempStrUV[2][32]={0};



	    BPTR fh;

	    	char lineRead[250]={0};

	    	UBYTE *buffer;

// char name[]="ship2.obj";
char name[]="kostka.obj";
//char name[]="icarus.obj";
//char name[]="i3.obj";
    fh = Open(name, MODE_OLDFILE);

    if (fh) 			{
	//	printf ("File open: %s\n", name);
		while (buffer = FGets(fh, lineRead, sizeof(lineRead)))
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





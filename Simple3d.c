#define INTUI_V36_NAMES_ONLY

#include <intuition/intuition.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <stdio.h>
#include <utility/tagitem.h>


#include "gameLoop.h"
//#include "globals.h"
#include "freeBitMaps.h"
#include "freePlanes.h"


/* characteristics of the screen */
#define SCR_WIDTH  (320)
#define SCR_HEIGHT (200)
#define SCR_DEPTH    (4)
//

/* Prototypes for our functions */
struct BitMap **setupBitMaps( LONG, LONG, LONG );
LONG   setupPlanes(struct BitMap *, LONG, LONG, LONG );

struct Window *win;
struct Library *IntuitionBase = NULL;
struct Library *GfxBase       = NULL;

/*
** Main routine.  Setup for using the double buffered screen.
** Clean up all resources when done or on any error.
*/

VOID main(int argc, char **argv)
{
struct BitMap **myBitMaps;
struct Screen  *screen;
struct NewScreen myNewScreen={0};



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


struct TagItem tags[2] = {{SA_Colors, (ULONG)cols},	{TAG_END, 0}};

//struct TagItem vcTags[] =
//{
//    {VTAG_ATTACH_CM_SET, NULL },
//    {VTAG_VIEWPORTEXTRA_SET, NULL },
//    {VTAG_NORMAL_DISP_SET, NULL },
//    {VTAG_END_CM, NULL }
//};



IntuitionBase = OpenLibrary("intuition.library", 33L);
if ( IntuitionBase != NULL )
    {
    GfxBase = OpenLibrary("graphics.library", 33L);
    if ( GfxBase != NULL )
        {
        myBitMaps = setupBitMaps(SCR_DEPTH, SCR_WIDTH, SCR_HEIGHT);
        if ( myBitMaps != NULL )
            {
            /* Open a simple quiet screen that is using the first
            ** of the two bitmaps.
            */
            myNewScreen.LeftEdge=0;
            myNewScreen.TopEdge=0;
            myNewScreen.Width=SCR_WIDTH;
            myNewScreen.Height=SCR_HEIGHT;
            myNewScreen.Depth=SCR_DEPTH;
            myNewScreen.DetailPen=0;
            myNewScreen.BlockPen=1;
            if( SCR_WIDTH ==320 && SCR_HEIGHT == 200 ) { myNewScreen.ViewModes = 0; };
            if( SCR_WIDTH ==320  && SCR_HEIGHT == 400) { myNewScreen.ViewModes = 0 | LACE ;};
            if( SCR_WIDTH ==640  && SCR_HEIGHT == 200) { myNewScreen.ViewModes = HIRES;};
            if( SCR_WIDTH ==640  && SCR_HEIGHT == 400) { myNewScreen.ViewModes = HIRES | LACE;};
            myNewScreen.Type=CUSTOMSCREEN | CUSTOMBITMAP | SCREENQUIET;
            myNewScreen.Font=NULL;
            myNewScreen.DefaultTitle=NULL;
            myNewScreen.Gadgets=NULL;
            myNewScreen.CustomBitMap=myBitMaps[0];

            screen = OpenScreenTagList(&myNewScreen, tags);




            if (screen != NULL)
                {


            	 win = OpenWindowTags(NULL,
            						            						WA_CustomScreen, screen,

            															WA_Borderless,   TRUE,
            															WA_Backdrop,     TRUE,
																		WA_Activate, TRUE,
            															WA_NoCareRefresh, TRUE,
            															WA_IDCMP,        IDCMP_RAWKEY,
            															WA_Flags,
            															WA_CloseGadget,  FALSE,
            															WA_RMBTrap, FALSE,
            															TAG_END);


            	 if ( win != NULL )
            	     {


                /* Indicate that the rastport is double buffered. */
                screen->RastPort.Flags = DBUFFER;

                gameLoop(&(screen->RastPort),screen, myBitMaps, win);

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



/*
** setupBitMaps(): allocate the bit maps for a double buffered screen.
*/
struct BitMap **setupBitMaps(LONG depth, LONG width, LONG height)
{
/* this must be static -- it cannot go away when the routine exits. */
static struct BitMap *myBitMaps[2];

myBitMaps[0] = (struct BitMap *) AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
if (myBitMaps[0] != NULL)
    {
    myBitMaps[1] = (struct BitMap *)AllocMem((LONG)sizeof(struct BitMap), MEMF_CLEAR);
    if (myBitMaps[1] != NULL)
        {
        InitBitMap(myBitMaps[0], depth, width, height);
        InitBitMap(myBitMaps[1], depth, width, height);

        if (NULL != setupPlanes(myBitMaps[0], depth, width, height))
            {
            if (NULL != setupPlanes(myBitMaps[1], depth, width, height))
                return(myBitMaps);

            freePlanes(myBitMaps[0], depth, width, height);
            }
        FreeMem(myBitMaps[1], (LONG)sizeof(struct BitMap));
        }
    FreeMem(myBitMaps[0], (LONG)sizeof(struct BitMap));
    }
return(NULL);
}




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

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>





freeBitMaps(struct BitMap **myBitMaps, LONG depth, LONG width, LONG height)
{
freePlanes(myBitMaps[0], depth, width, height);
freePlanes(myBitMaps[1], depth, width, height);

FreeMem(myBitMaps[0], (LONG)sizeof(struct BitMap));
FreeMem(myBitMaps[1], (LONG)sizeof(struct BitMap));
}

/*
** setupPlanes(): allocate the bit planes for a screen bit map.
*/
//LONG setupPlanes(struct BitMap *bitMap, LONG depth, LONG width, LONG height)
//{
//SHORT plane_num ;
//
//for (plane_num = 0; plane_num < depth; plane_num++)
//    {
//    bitMap->Planes[plane_num] = (PLANEPTR)AllocRaster(width, height);
//    if (bitMap->Planes[plane_num] != NULL )
//        BltClear(bitMap->Planes[plane_num], (width / 8) * height, 1);
//    else
//        {
//        freePlanes(bitMap, depth, width, height);
//        return(NULL);
//        }
//    }
//return(TRUE);
//}

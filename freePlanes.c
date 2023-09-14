#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

/*
** freePlanes(): free up the memory allocated by setupPlanes().
*/
freePlanes(struct BitMap *bitMap, LONG depth, LONG width, LONG height)
{
SHORT plane_num ;

for (plane_num = 0; plane_num < depth; plane_num++)
    {
    if (bitMap->Planes[plane_num] != NULL)
        FreeRaster(bitMap->Planes[plane_num], width, height);
    }
}

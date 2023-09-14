#include <intuition/intuition.h>
#include <stdio.h>
//#include "gameLoop.h"
#include "globals.h"

struct IntuiMessage *msg;

ULONG WindowGetSignal(struct Window *win)
{

    windowSignalGame = 0L;

 //   printf("IDCMP_RAWKEY: 0x%02X\n", windowSignalGame); // Wy�wietlanie na konsoli

    while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort))) {
        switch (msg->Class) {
            case IDCMP_RAWKEY:
                // Obs�uga sygna��w IDCMP_RAWKEY
                windowSignalGame = msg->Code;
//                printf("IDCMP_RAWKEY: 0x%02X\n", windowSignalGame); // Wy�wietlanie na konsoli
                break;
            // Dodaj obs�ug� innych rodzaj�w sygna��w IDCMP, je�li jest to potrzebne
        }
        ReplyMsg((struct Message *)msg);
    }

    return windowSignalGame;
}

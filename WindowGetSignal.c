#include <intuition/intuition.h>
#include <stdio.h>
//#include "gameLoop.h"
#include "globals.h"

struct IntuiMessage *msg;

ULONG WindowGetSignal(struct Window *win)
{

    windowSignalGame = 0L;

 //   printf("IDCMP_RAWKEY: 0x%02X\n", windowSignalGame); // Wyœwietlanie na konsoli

    while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort))) {
        switch (msg->Class) {
            case IDCMP_RAWKEY:
                // Obs³uga sygna³ów IDCMP_RAWKEY
                windowSignalGame = msg->Code;
//                printf("IDCMP_RAWKEY: 0x%02X\n", windowSignalGame); // Wyœwietlanie na konsoli
                break;
            // Dodaj obs³ugê innych rodzajów sygna³ów IDCMP, jeœli jest to potrzebne
        }
        ReplyMsg((struct Message *)msg);
    }

    return windowSignalGame;
}

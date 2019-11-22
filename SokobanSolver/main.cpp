/************************************************************
Sokoban project - Main file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/
#include "menu.h"
Graphic g(1024,650,32);

int main()
{
    // Do not touch !
    Console::getInstance()->setColor(_COLOR_DEFAULT);

    // Buffer
    BITMAP *page;
    if (g.init())
    {
        install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0);
        musique();
        page=create_bitmap(SCREEN_W,SCREEN_H);
        menu(page, g);
    }

    // Free memory
    g.deinit();
    Console::deleteInstance();

    return 0;
}
END_OF_MAIN()

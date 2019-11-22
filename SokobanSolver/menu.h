#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "maze.h"
#include "graphic.h"
#include "utils/console.h"
#include "utils/coord.h"
#include <iostream>
#include <allegro.h>

// Init allegro with 1024x650 (resolution)

void musique ();

BITMAP * load_bitmap_check(std::string ch);

bool inGame(std::string path, Graphic &g);

void chargement_niveau(BITMAP *page, BITMAP *VISEUR, Graphic &g);

void menu(BITMAP *page, Graphic &g);




#endif // MENU_H_INCLUDED

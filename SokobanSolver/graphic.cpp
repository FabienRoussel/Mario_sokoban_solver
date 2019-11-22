/************************************************************
Sokoban project - Graphic file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/

#include "graphic.h"

Graphic::Graphic(int _resX, int _resY, int _depth)
    : pBuffer(NULL), m_resX(_resX), m_resY(_resY), m_depth(_depth)
{
    for (int i=0; i<MAX_SPRITE; i++)
        pSprites[i] = NULL;
    for (int i=0; i<MAX_SPRITE_PLAYER; i++)
        pSpritesPlayer[i] = NULL;
}

Graphic::~Graphic()
{
}

bool Graphic::_imageLoadSprites()
{
    this->pBuffer = imageCreate(m_resX, m_resY, COLOR_WHITE);
    if (!this->pBuffer)
    {
        std::cerr << "Cannot allocate buffer..." << std::endl;
        return 0;
    }

    // chargement des bitmaps du jeu
    this->pSprites[0] = imageCreate(TX, TY, COLOR_WHITE);
    this->pSprites[1] = imageLoad("images/mur.bmp");
    this->pSprites[2] = imageLoad("images/caisse.bmp");
    this->pSprites[3] = imageLoad("images/caisse_ok.bmp");
    this->pSprites[4] = imageLoad("images/objectif.bmp");
    this->pSprites[5] = imageCreate(TX, TY, COLOR_ROSE);
    this->pSprites[6] = imageCreate(TX, TY, COLOR_BLUE);
    this->pSprites[7] = imageCreate(TX, TY, COLOR_YELLOW);
    this->pSprites[8] = imageCreate(TX, TY, COLOR_GREEN);
    this->pSprites[9] = imageCreate(TX, TY, COLOR_RED);

    this->pSpritesPlayer[0] = imageLoad("images/mario_haut.bmp");
    this->pSpritesPlayer[1] = imageLoad("images/mario_bas.bmp");
    this->pSpritesPlayer[2] = imageLoad("images/mario_gauche.bmp");
    this->pSpritesPlayer[3] = imageLoad("images/mario_droite.bmp");

    for (int i=0; i<MAX_SPRITE; i++)
    {
        if (this->pSprites[i] == NULL)
        {
            return false;
        }
    }
    for (int i=0; i<MAX_SPRITE_PLAYER; i++)
    {
        if (this->pSpritesPlayer[i] == NULL)
        {
            return false;
        }
    }

    return true;
}

bool Graphic::init()
{
    set_color_depth(m_depth); // profondeur des couleurs 8, 16, 24 ou 32
    set_uformat(U_ASCII); // pour gérer les accents

    allegro_init();

    if (install_mouse() == -1)
    {
        std::cerr << ("Failed to install mouse") << std::endl;
        return false;
    }
    if (install_keyboard() != 0)
    {
        std::cerr << ("Failed to install keyboard") << std::endl;
        return false;
    }
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, m_resX, m_resY, 0, 0) != 0)
    {
        std::cerr << ("Failed to enter in graphic mode") << std::endl;
        return false;
    }

    set_keyboard_rate(500, 100);

    show_mouse(screen);

    return this->_imageLoadSprites();
}

void Graphic::deinit()
{
    for (int i=0; i<MAX_SPRITE; i++)
    {
        imageDestroy(this->pSprites[i]);
    }
    for (int i=0; i<MAX_SPRITE_PLAYER; i++)
    {
        imageDestroy(this->pSpritesPlayer[i]);
    }

    imageDestroy(pBuffer);
    remove_mouse();
    remove_keyboard();
    allegro_exit();
}

pSurface Graphic::imageLoad(const std::string& fileName)
{
    return load_bitmap(fileName.c_str(), NULL);
}

pSurface Graphic::imageCreate(int width, int height, int color)
{
    pSurface s = NULL;
    s = create_bitmap(width, height);
    if (s != NULL)
        imageClearToColor(s, color);

    return s;
}

void Graphic::imageDestroy(pSurface s)
{
    destroy_bitmap(s);
}

void Graphic::imageClearToColor(const pSurface s, int color)
{
    clear_to_color(s, color);
}

void Graphic::clear()
{
    imageClearToColor(pBuffer, COLOR_WHITE);
}

void Graphic::drawCircle(int x, int y, int radius, int color, int fill, bool coord) const
{
    if (coord)
    {
        x = x*TX + TX/2;
        y = y*TY + TY/2;
    }

    if (fill)
        circlefill(pBuffer, x, y, radius, color);
    else circle(pBuffer, x, y, radius, color);
}

void Graphic::drawRect(int x1, int y1, int x2, int y2, int color, int fill, bool coord) const
{
    if (coord)
    {
        x1 *= TX;
        y1 *= TY;
        x2 *= TX;
        y2 *= TY;
    }

    if (fill==1)
        rectfill(pBuffer, x1, y1, x2, y2, color);
    else if (fill > 1)
    {
        rect(pBuffer, x1-1, y1-1, x2+1, y2+1, color);
        rect(pBuffer, x1, y1, x2, y2, color);
        rect(pBuffer, x1+1, y1+1, x2-1, y2-1, color);
    }
    else rect(pBuffer, x1, y1, x2, y2, color);
}

void Graphic::drawT(const pSurface src, int x, int y, bool coord) const
{
    if (coord)
    {
        x *= TX;
        y *= TY;
    }
    draw_sprite(pBuffer, src, x, y);
}

void Graphic::draw(const pSurface src, int source_x, int source_y, int dest_x, int dest_y, int width, int height, bool coord) const
{
    if (coord)
    {
        source_x *= TX;
        source_y *= TY;
        dest_x *= TX;
        dest_y *= TY;
    }
    blit(src, pBuffer, source_x, source_y, dest_x, dest_y, width, height);
}

void Graphic::display(int nbCol, bool withGrid)
{
    if (withGrid) this->_grid(COLOR_BLACK);
    //draw_sprite(screen, pBuffer, 0, 0);

    if (nbCol > 0)
    {
        int col = mouse_x / TX;
        int lig = mouse_y / TY;
        textprintf_ex(pBuffer, font, mouse_x, mouse_y, COLOR_BLUE, -1,
            "Pos = %d", (lig * nbCol + col));
    }

    blit(pBuffer, screen, 0, 0, 0, 0, pBuffer->w, pBuffer->h);

    rest(20); // tempo
}

void Graphic::cursor(int l, int c, int color)
{
    drawRect(c, l, (c+1), (l+1), color, 0);
}

void Graphic::_grid(int color)
{
    int nbL = 25; //pBuffer->w/TX;
    int nbC = pBuffer->h/TY;
    for(int i=0; i<nbL; i++)
        for(int j=0; j<nbC; j++)
            drawRect(i, j, (i+1), (j+1), color, 0);
}


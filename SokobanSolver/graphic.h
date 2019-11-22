/************************************************************
Sokoban project - Graphic file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/

#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

#include <allegro.h>
#include <string>
#include <iostream>

// Image size
#define TX 24
#define TY 24

#define MAX_SPRITE           10
#define MAX_SPRITE_PLAYER    4

#define COLOR_WHITE     makecol(255,255,255)
#define COLOR_BLACK     makecol(0,0,0)
#define COLOR_RED       makecol(255,0,0)
#define COLOR_GREEN     makecol(0,255,0)
#define COLOR_BLUE      makecol(0,0,255)
#define COLOR_YELLOW    makecol(255,255,0)
#define COLOR_CYAN      makecol(0,255,255)
#define COLOR_ROSE      makecol(244,194,194)

// Game inputs
#define ARROW_UP          84 // KEY_UP equivalent
#define ARROW_BOTTOM      85
#define ARROW_LEFT        82
#define ARROW_RIGHT       83

typedef struct BITMAP  Surface;
typedef struct BITMAP* pSurface;

class Graphic
{
    private:
        pSurface pSprites[MAX_SPRITE];
        pSurface pSpritesPlayer[MAX_SPRITE_PLAYER];
        pSurface pBuffer; // image buffer
        int m_resX, m_resY; // image resolution
        int m_depth; // depth for image

        bool _imageLoadSprites();
        void _grid(int color);

    public:
        Graphic(int, int, int);
        ~Graphic();

        // Init/Deinit
        bool init();
        void deinit();

        // Image manipulation
        pSurface imageLoad(const std::string& filename);
        pSurface imageCreate(int, int, int);
        void imageClearToColor(const pSurface s, int color);
        void imageDestroy(pSurface);

        // Drawing and primitives
        void drawCircle(int x, int y, int radius, int color, int fill, bool coord = true) const;
        void drawRect(int x1, int y1, int x2, int y2, int color, int fill, bool coord = true) const;
        void drawT(const pSurface src, int x, int y, bool coord = true) const;
        void draw(const pSurface src, int source_x, int source_y, int dest_x, int dest_y, int width, int height, bool coord = true) const;
        void display(int nbCol = 0, bool withGrid = true);
        void clear();

        // Other
        void cursor(int l, int c, int color);

        // Getter/setter
        int keyPressed() const;
        int keyGet(int code) const;
        int keyRead() const;
        pSurface getSprite(int s) const ;
        pSurface getSpritePlayer(int s) const;
};

inline int Graphic::keyPressed() const { return keypressed(); }
inline int Graphic::keyGet(int code) const { return key[code]; }
inline int Graphic::keyRead() const { return (readkey() >> 8); }

inline pSurface Graphic::getSprite(int s) const { return pSprites[s]; }
inline pSurface Graphic::getSpritePlayer(int s) const { return pSpritesPlayer[s]; }

#endif // GRAPHIC_H_INCLUDED

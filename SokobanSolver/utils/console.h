#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include <iostream>

enum Color
{
    _COLOR_BLACK = 0,
    _COLOR_WHITE = 1,
    _COLOR_RED = 2,
    _COLOR_GREEN = 3,
    _COLOR_BLUE = 4,
    _COLOR_YELLOW = 5,
    _COLOR_PURPLE = 6,
    _COLOR_DEFAULT = 7, // gris couleur de base de la console
};

class Console
{
    private:
        // Empecher la cr�ation
        __attribute__((no_instrument_function))
        Console();

        __attribute__((no_instrument_function))
        ~Console();

        // Empecher la copie d'objet...
        Console& operator= (const Console&){ return *this;}
        Console (const Console&){}

        // Attributs
        static Console* m_instance;

        // M�thodes priv�es
        __attribute__((no_instrument_function))
        void _setColor(short unsigned int front, short unsigned int back);
    public:
        // M�thodes statiques (publiques)
        __attribute__((no_instrument_function))
        static Console* getInstance();

        __attribute__((no_instrument_function))
        static void deleteInstance();

        // M�thodes publiques
        __attribute__((no_instrument_function))
        void gotoLigCol(short int lig, short int col);

        __attribute__((no_instrument_function))
        void gotoCurrentLigCol(short int lig, short int col);

        __attribute__((no_instrument_function))
        void gotoCol(short int col);

        __attribute__((no_instrument_function))
        bool isKeyboardPressed();

        __attribute__((no_instrument_function))
        int getInputKey();

        __attribute__((no_instrument_function))
        void setColor(Color col);
};

#endif // CONSOLE_H_INCLUDED

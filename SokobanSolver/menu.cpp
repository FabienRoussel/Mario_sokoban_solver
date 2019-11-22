#include "menu.h"

void musique()
{
    MIDI* musique;

    musique=load_midi("Musiques/them.mid");
    if(!musique) std::cout << "error" <<std::endl;

    play_midi(musique,10);
}

BITMAP * load_bitmap_check(std::string ch)
{
    BITMAP *chargement;
    // charger image de fond
    chargement=load_bitmap(ch.c_str(),NULL);
    if (!chargement)
    {
        allegro_message("pas pu trouver %s", ch.c_str());
        exit(EXIT_FAILURE);
    }
    return chargement;
}

bool inGame(std::string path, Graphic &g)
{
    // Do not touch !
    Console::getInstance()->setColor(_COLOR_DEFAULT);
    unsigned short SizeMat;
    std::vector<char> direction;
    bool DeadLockStat=false;
    bool music=true;
    unsigned short msec=150;
    ///Variable Test

    // Load level from a fill
    Maze m(path.c_str());
    if (!m.init()) return -1;
    std::cout << m << std::endl;
    bool win = false;
    unsigned short numberCaseM=(m.getField().size());

    // While playing...
    while (!g.keyGet(KEY_ESC) && !win)
    {
        // Check if user has pressed a key
        if (g.keyPressed())
        {
            //Ajout de code entre  /// pour tester les différentes fonctions
            ///
            for(unsigned short i=0; i<numberCaseM; i++)
            {
                if(m.isSquareDead(i) && !DeadLockStat) m.setSquare(i, SPRITE_GROUND);
            }
            ///
            switch (g.keyRead())
            {
                //Pour se déplacer vers le haut
                case ARROW_UP:
                    win = m.updatePlayer(TOP);
                    break;

                //Pour se déplacer vers le bas
                case ARROW_BOTTOM:
                    win = m.updatePlayer(BOTTOM);
                    break;

                //Pour se déplacer vers la droite
                case ARROW_RIGHT:
                    win = m.updatePlayer(RIGHT);
                    break;

                //Pour se déplacer vers la gauche
                case ARROW_LEFT:
                    win = m.updatePlayer(LEFT);
                    break;

                //Pour changer la vitesse
                case KEY_V:
                    if(msec<200)
                        msec+=50;
                    else msec+=150;
                    if(msec>500) msec=50;
                    std::cout << msec <<std::endl;
                    break;

                //Si F on fait le brute force
                case KEY_F:
                    direction=m.Maze::Bruteforce(win, direction, g);
                    SizeMat=direction.size()+2;
                    for (unsigned short i=SizeMat; i>0;i--)
                    {
                        win=m.Maze::updatePlayer(direction[i-1]);
                        g.clear();
                        m.draw(g);
                        g.display(Coord::m_nb_col);
                        rest(250);
                    }
                    win=m._isFinish();
                    break;

                //si B on fait le BFS
                case KEY_B:
                    m.solvBFS();
                    m.displayPath(g, msec);
                    win=m._isFinish();
                    break;

                //si D on fait le DFS
                case KEY_D:
                    m.solvDFS();
                    m.displayPath(g, msec);
                    win=m._isFinish();
                    break;

                // si I on fait le Iterative Depth A*
                case KEY_I:
                    m.solvIDA();
                    m.displayPath(g, msec);
                    win=m._isFinish();
                    break;

                // si O on fait le Iterative Depth A*
                case KEY_O:
                    m.solvBestFS();
                    m.displayPath(g, msec);
                    win=m._isFinish();
                    break;

                //si A on fait le BestFS
                case KEY_Q:
                    m.pBestFS();
                    m.displayPath(g, msec);
                    win=m._isFinish();
                    break;

                //si L on met (ou pas) de la musique
                case KEY_L:
                    if(music)
                    {
                        music=false;
                        stop_midi();
                    }
                    else{
                        musique();
                        music=true;
                    }
                    break;

                //si K en qwerty alors on affiche les dead lock
                case KEY_K:
                    if (DeadLockStat==false)
                        DeadLockStat=true;
                    else {
                        DeadLockStat=false;
                        for(unsigned short i=0; i<numberCaseM; i++)
                        {
                            if(m.isSquareDead(i)) m.setSquare(i, SPRITE_GROUND);
                        }
                    }
                    break;

                //si J en qwerty alors on affiche les dead lock
                case KEY_J:
                    m.setActDL();
                    break;
            }
            if (DeadLockStat) m.deadlocksStatic();
            if (win) std::cout << "Win ! " << std::endl;
        }

        // Display on screen
        g.clear();
        m.draw(g);
        g.display(Coord::m_nb_col);
    }

    rest(500);
    return win;
}

void chargement_niveau(BITMAP *page, BITMAP *VISEUR, Graphic &g)
{
    rest(150);
    clear_keybuf();
    char choice='a';
    BITMAP* fond_NIVEAU;
    bool win=false;
    fond_NIVEAU =load_bitmap_check("images/Jeu.bmp");
    while (!key[KEY_ESC] && choice)
    {
        rest(10);
        // effacer buffer en appliquant décor  (pas de clear_bitmap)
        blit(fond_NIVEAU,page,0,0,0,0,SCREEN_W,SCREEN_H);
        draw_sprite(page, VISEUR, mouse_x-VISEUR->w/2, mouse_y-VISEUR->h/2);
        // afficher tout ça à l'écran
        blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);

        //1. Affichage du menu
        if(mouse_b&1)
        {
            // level easy
            if(mouse_y>165 && mouse_y<230)
            {
                // 1
                if(mouse_x>40 && mouse_x<105)  choice='1';
                // 2
                else if( mouse_x>137   && mouse_x<203)  choice='2';

                // 3
                else if(mouse_x>230  && mouse_x<300)  choice='3';

                // 4
                else if(mouse_x>328 && mouse_x<396)  choice='4';

                // 5
                else if(mouse_x>425   && mouse_x<493)  choice='5';

                // 6
                else if(mouse_x>520  && mouse_x<588)  choice='6';

                // 7
                else if(mouse_x>617  && mouse_x<685)  choice='7';

                // 8
                else if(mouse_x>714 && mouse_x<782)  choice='8';

                // 9
                else if(mouse_x>811   && mouse_x<879)  choice='9';

                // 10
                else if(mouse_x>907  && mouse_x<990)  choice=10;
            }
            // Medium
            // 1
            else if(mouse_x>40 && mouse_x<105 && mouse_y>300 && mouse_y<365)  choice=11;

            // 2
            else if(mouse_x>137   && mouse_x<203 && mouse_y>300 && mouse_y<365)  choice=12;

            // 3
            else if(mouse_x>230  && mouse_x<300 && mouse_y>300 && mouse_y<365)  choice=13;

            // Hard
            // 1
            else if(mouse_b&1 &&
                ( mouse_x>40 && mouse_x<105 && mouse_y>430 && mouse_y<495) ) choice=21;

            // 2
            else if(mouse_b&1 &&
                ( mouse_x>137   && mouse_x<203 && mouse_y>430 && mouse_y<495) ) choice=22;

            // 3
            else if(mouse_b&1 &&
                ( mouse_x>230  && mouse_x<300 && mouse_y>430 && mouse_y<495) ) choice=23;

            // TEST 1
            else if(mouse_b&1 &&
                ( mouse_x>108  && mouse_x<135 && mouse_y>555 && mouse_y<588) ) choice=30;

            // 2
            else if(mouse_b&1 &&
                ( mouse_x>140  && mouse_x<170 && mouse_y>555 && mouse_y<588) ) choice=31;

            // Quitter
            else if(mouse_b&1 &&
                ( mouse_x>40  && mouse_x<232 && mouse_y>20 && mouse_y<60) ) choice='0';

            else choice='a';

            //3. Analyse du choix
            switch (choice)
            {
                //3.1. Choix du niveau 1
                case '1' :
                    win=inGame("./levels/easy/easy_1.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.2. Choix du niveau 2
                case '2' :
                    win=inGame("./levels/easy/easy_2.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.3. Choix du niveau 3
                case '3' :
                    win=inGame("./levels/easy/easy_3.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.4. Choix du niveau 4
                case '4' :
                    win=inGame("./levels/easy/easy_4.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.5. Choix du niveau 5
                case '5' :
                    win=inGame("./levels/easy/easy_5.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.6. Choix du niveau 6
                case '6' :
                    win=inGame("./levels/easy/easy_6.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.7. Choix du niveau 7
                case '7' :
                    win=inGame("./levels/easy/easy_7.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.8. Choix du niveau 8
                case '8' :
                    win=inGame("./levels/easy/easy_8.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.9. Choix du niveau 9
                case '9' :
                    win=inGame("./levels/easy/easy_9.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.10. Choix du niveau 10
                case 10 :
                    win=inGame("./levels/easy/easy_10.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.11. Choix du niveau Medium 1
                case 11 :
                    inGame("./levels/medium/medium_1.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.12. Choix du niveau Medium 2
                case 12 :
                    win=inGame("./levels/medium/medium_2.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.13. Choix du niveau Medium 3
                case 13 :
                    win=inGame("./levels/medium/medium_3.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.21. Choix du niveau Hard 1
                case 21 :
                    win=inGame("./levels/hard/hard_1.dat", g);
                    rest( mouse_x>40 && mouse_x<105200);
                    clear_keybuf();
                    break;

                //3.22. Choix du niveau Hard 2
                case 22 :
                    win=inGame("./levels/hard/hard_2.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.23. Choix du niveau Hard 3
                case 23 :
                    win=inGame("./levels/hard/hard_3.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.001. Choix de retourner au menu
                case 30 :
                    win=inGame("./levels/test/test_1.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.001. Choix de retourner au menu
                case 31 :
                    win=inGame("./levels/test/test_3.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.001. Choix de retourner au menu
                case '0' : choice=0;
                    break;

                //3.69. Cas d'une saisie erronée
                default :
                    break;            }
            //Si le niveau a été réussi
            if (win)
            {
                BITMAP* WINNER;
                WINNER =load_bitmap_check("images/star.bmp");
                draw_sprite(page, WINNER,SCREEN_W/2-WINNER->w/2,SCREEN_H/2-WINNER->h/2);
                blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
                rest(2000);
                win=false;
            }
        }
        system("CLS");
    }
    rest(200);
}

void menu(BITMAP *page, Graphic &g)
{
    //0. Déclaration des ressources locales
    char choice='a';
    BITMAP* fond_menu;
    fond_menu =load_bitmap_check("images/marioSokoban.bmp");
    BITMAP* go_charger;
    go_charger =load_bitmap_check("images/charger.bmp");
    BITMAP* VISEUR;
    VISEUR =load_bitmap_check("images/champi.bmp");

    while (!key[KEY_ESC] && choice)
    {
        rest(10);
        // effacer buffer en appliquant décor  (pas de clear_bitmap)
        blit(fond_menu,page,0,0,0,0,SCREEN_W,SCREEN_H);

        draw_sprite(page, go_charger,SCREEN_W/2-go_charger->w/2-20,520);
        draw_sprite(page, VISEUR, mouse_x-VISEUR->w/2, mouse_y-VISEUR->h/2);
        // afficher tout ça à l'écran
        blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);

        //1. Affichage du menu
        if(mouse_b&1)
        {
            if ( mouse_x>164   && mouse_x<330 && mouse_y>577 && mouse_y<617)  choice='1';
            else if( mouse_x>SCREEN_W/2-go_charger->w/2-20 && mouse_x<SCREEN_W/2+go_charger->w/2-20 &&
                    mouse_y>520 && mouse_y<520+go_charger->h ) choice='2';
            else if( mouse_x>636 && mouse_x<850 && mouse_y>577 && mouse_y<618)  choice='6';
            else choice='a';
            //3. Analyse du choix
            switch (choice)
            {
                //3.1. Choix du lancement d'une nouvelle partie (même si le joueur est à un niveau supérieur)
                case '1' :
                    inGame("./levels/medium/medium_1.dat", g);
                    rest(200);
                    clear_keybuf();
                    break;

                //3.2. Choix du chargement des niveaux
                case '2' :
                    chargement_niveau(page, VISEUR, g);
                    rest(250);
                    clear_keybuf();
                    break;

                //3.6. Choix de quitter le jeu
                case '6' :
                    choice=0;
                    rest(250);
                    clear_keybuf();
                    break;

                //3.7. Cas d'une saisie erronée
                default :
                    break;            }
        }
        system("CLS");
    }

    //4. Lorsque le jeu se termine
    rest(150);
    clear_keybuf() ;
}

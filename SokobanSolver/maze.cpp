/************************************************************
Sokoban project - Maze file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/

#include "maze.h"
#include "graphic.h"
#include "utils/console.h"
#include "utils/coord.h"
#include <fstream>
#include <iomanip>
#include <cmath>

Maze::Maze(const std::string& path)
    : m_lig(0), m_col(0), m_pos_player(0), m_dir_player(TOP), m_level_path(path), m_actifDeadlock(false)
{
}

Maze::~Maze()
{
}

// Load a maze from a file (DO NOT TOUCH)
bool Maze::_load(const std::string& path)
{
    std::vector<unsigned short> tmpPosBoxes;
    std::ifstream ifs(path.c_str());
    if (ifs)
    {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(ifs, line))
        {
            lines.push_back(line);
            this->m_lig++;
            this->m_col = (this->m_col < line.size() ? line.size() : this->m_col);
        }
        ifs.close();

        if (this->m_col > NB_MAX_WIDTH || this->m_lig > NB_MAX_HEIGHT)
        {
            std::cerr << "Maze::load => Bad formatting in level data..." << std::endl;
            return false;
        }

        Coord::m_nb_col = this->m_col;
        for (unsigned int i=0; i<lines.size(); i++)
        {
            //LDebug << "Maze::load => Reading : " << lines[i];
            for (unsigned int j=0; j<this->m_col; j++)
            {
                if (j < lines[i].size())
                {
                    bool both = false;
                    unsigned short pos = Coord::coord1D(i, j);
                    unsigned char s = (unsigned char)(lines[i][j] - '0');

                    // Need to add a goal and a box ;)
                    if (s == SPRITE_BOX_PLACED)
                    {
                        both = true;
                    }
                    if (s == SPRITE_GOAL || both)
                    {
                        this->m_pos_goals.push_back(pos);
                    }
                    if (s == SPRITE_BOX || both)
                    {
                        tmpPosBoxes.push_back(pos);
                    }

                    // Assign player position
                    if (s == SPRITE_MARIO)
                    {
                        this->m_pos_player = pos;
                        //LDebug << "\tAdding player pos (" << pos << ")";
                        s = SPRITE_GROUND;
                    }
                    // Add this value in the field
                    this->m_field.push_back(s);
                }
                else
                {
                    // Here - Out of bound
                    this->m_field.push_back(SPRITE_GROUND);
                }
            }
        }

        // Copy box position
        this->m_pos_boxes.resize(tmpPosBoxes.size());
        for (unsigned int i=0; i<tmpPosBoxes.size(); ++i)
        {
            this->m_pos_boxes[i] = tmpPosBoxes[i];
        }

        return (this->m_pos_boxes.size() == this->m_pos_goals.size());
    }
    else
    {
        std::cerr << "Maze::load => File does not exist..." << std::endl;
    }

    return false;
}

bool Maze::init()
{
    bool res = this->_load(this->m_level_path);
    if (!res)
    {
        //Si on a pas réussi a ouvrir le fichier
        std::cerr << "Cannot load maze... Check file : " << this->m_level_path << std::endl;
        return false;
    }

    return res;
}

// Check if all boxes are on a goal
bool Maze::_isCompleted() const
{
    for (unsigned int i=0; i<this->m_pos_boxes.size(); ++i)
    {
        if (!this->isSquareBoxPlaced(this->m_pos_boxes[i]))
            return false;
    }
    return true;
}

// Check if we can push a box in a direction
// INPUT: position of the box to check, direction,
// OUTPUT : the position of the box after pushing
//      TRUE if all goes right otherwise FALSE
bool Maze::_canPushBox(unsigned short posBox, char dir, unsigned short& newPosBox) const
{
    // Check if this position is a box !
    if (!this->isSquareBox(posBox))
        return false;

    // Compute new position according to push direction
    newPosBox = Coord::getDirPos(posBox, dir);

    // Can we push the box ?
    return (this->isSquareWalkable(newPosBox) && !this->isSquareDead(newPosBox));
}

void Maze::swapTwoSquare(unsigned short pos1, unsigned short pos2)
{
    this->setSquare(pos1, 0);
    this->setSquare(pos2, 2);
    for(unsigned short i=0; i< this->getGoals().size(); i++)
    {
        if (!this->isSquareBox(getGoals()[i]) && !this->isSquareBoxPlaced(getGoals()[i]))
            this->setSquare(getGoals()[i], 4);
        else this->setSquare(getGoals()[i], 3);
    }

}

//Gerer les déplacement du joueur
bool Maze::updatePlayer(char dir)
{
    short unsigned int posAim=0;
    short unsigned int posBox=0;
    //Si la touche n'est pas valide
    if (dir < 0 || dir > MAX_DIR)
    {
        ///std::cerr << "Maze::updatePlayer => Direction not correct... " << +dir << std::endl;
        return false;
    }
    // Implement player moving and pushing here
    switch (dir)
    {
    //On actualise la position souhaitée et le bitmap du joueur pour aller ..
    // .. en Haut
    case TOP:
        posAim = this->getPosPlayer() - this->getCol();
        m_dir_player = TOP;
        break;

    // .. en Bas
    case BOTTOM:
        posAim = this->getPosPlayer() + this->getCol();
        m_dir_player = BOTTOM;
        break;

    // .. à Gauche
    case LEFT:
        posAim = this->getPosPlayer() - 1 ;
        m_dir_player = LEFT;
        break;

    // .. à Droite
    case RIGHT:
        posAim = this->getPosPlayer() +1;
        m_dir_player = RIGHT;
        break;
    }
    //Pour déplacer une case on regarde ce qu'il y a
    if(this->_canPushBox(posAim, dir, (posBox)) )
    {
        //On "echange" (pas un vrai echange) deux cases
        this->swapTwoSquare(posAim, posBox);
        //On actualise nos coordonnées dans notre vecteur
        for(short unsigned int i = 0; i != getPosBoxes().size(); ++i)
        {
            if (getPosBoxes()[i]==posAim)
            {
                this->setPosBox(i, posBox);
            }
        }
    }

    //Pour faire avancer le joueur on test ce qu'il  y a la ou il veut aller
    if (this->isSquareWalkable(posAim) || this->isSquareDead(posAim))
    {
        // On actualise ses positions
        this->setPlayerPos(posAim);
    }

    // On regarde si toutes les caisses sont sur leur goal
    return this->_isCompleted();
}

// Display maze on screen with Allegro
void Maze::draw(const Graphic& g) const
{
    for(unsigned int i=0; i<this->getSize(); i++)
    {
        unsigned int l = 0, c = 0;
        Coord::coord2D(i, l, c);

        if (i == this->m_pos_player)
        {
            g.drawT(g.getSpritePlayer(this->m_dir_player), c, l);
        }
        else
        {
            g.drawT(g.getSprite(this->m_field[i]), c, l);
        }
    }
}

// DO NOT TOUCH !
// Overload function for displaying debug information
// about Maze class
std::ostream& operator << (std::ostream& O, const Maze& m)
{
    unsigned int l, c;
    int i = 0;
    Coord::coord2D(m.m_pos_player, l, c);
    O << "Player position " << m.m_pos_player << " (" << l << "," << c << ")" << std::endl;
    O << "Field Size " << +m.m_lig << " x " << +m.m_col << " = " << m.getSize() << std::endl;
    O << "Field Vector capacity : " << m.m_field.capacity() << std::endl;
    O << "Field array : " << std::endl << std::endl;
    for(unsigned int l=0; l<m.getSize(); l++)
    {
        if (l == m.m_pos_player) Console::getInstance()->setColor(_COLOR_YELLOW);
        else if (m.isSquareWall(l)) Console::getInstance()->setColor(_COLOR_PURPLE);
        else if (m.isSquareBoxPlaced(l) || m.isSquareGoal(l)) Console::getInstance()->setColor(_COLOR_GREEN);
        else if (m.isSquareBox(l)) Console::getInstance()->setColor(_COLOR_BLUE);
        else if (m.m_field[l] == SPRITE_DEADSQUARE) Console::getInstance()->setColor(_COLOR_RED);
        else Console::getInstance()->setColor(_COLOR_WHITE);

        O << std::setw(2) << +m.m_field[l] << " "; // + => print as "int"

        if ((l+1) % m.m_col == 0)
        {
            O << std::endl;
        }
    }
    Console::getInstance()->setColor(_COLOR_DEFAULT);

    O << std::endl;
    O << "Box position : " << std::endl;
    for (unsigned int i=0; i<m.m_pos_boxes.size(); i++)
    {
        Coord::coord2D(m.m_pos_boxes[i], l, c);
        O << "\t" << "Box #" << i << " => " << std::setw(3) << m.m_pos_boxes[i] << std::setw(2) << " (" << l << "," << c << ")" << std::endl;
    }

    O << std::endl;
    O << "Goal position : " << std::endl;
    for (const auto& goal : m.m_pos_goals)
    {
        unsigned int l, c;
        Coord::coord2D(goal, l, c);
        if (m.isSquareBoxPlaced(goal)) Console::getInstance()->setColor(_COLOR_GREEN);
        O << "\t" << "Goal #" << i << " => " << std::setw(3) << goal << std::setw(2) << " (" << l << "," << c << ")" << std::endl;
        if (m.isSquareBoxPlaced(goal)) Console::getInstance()->setColor(_COLOR_DEFAULT);
        i++;
    }

    return O;
}

//Pour faire une courte pause
void Maze::sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}
/*
//Permet de savoir si une case a une certaine position est une deadLock
bool Maze::isSquareDeadlock(short unsigned posBox, short unsigned dir)
{
    //On calcule la position visée
    short unsigned newPos=Coord::getDirPos(posBox,dir);

    //On regarde si on ne vas pas sur un goal car un goal est un objectif donc pas une deadlock
    if(!isSquareGoal(newPos))
    {
        //Si la direction est vers le haut
        if(dir==TOP)
        {
            // On regarde si la case juste devant est walkable
            if(!isSquareWalkable(Coord::getDirPos(newPos,TOP)))
            {
                //si non on regarde les cases adjacentes a la case visée
                if( !isSquareWalkable(Coord::getDirPos(newPos,LEFT) ) || !isSquareWalkable(Coord::getDirPos(newPos,RIGHT) ) )
                {
                    //Maze::setSquare(newPos, SPRITE_DEADSQUARE);
                    return true;
                }
            }
        }
        //Si la direction est vers le bas
        else if (dir==BOTTOM)
        {
            // On regarde si la case juste avant est walkable
            if(!isSquareWalkable(Coord::getDirPos(newPos,BOTTOM)))
            {
                //si non on regarde les cases adjacentes a la case visée
                if( !isSquareWalkable(Coord::getDirPos(newPos,LEFT) ) || !isSquareWalkable(Coord::getDirPos(newPos,RIGHT) ) )
                {
                    //Maze::setSquare(newPos, SPRITE_DEADSQUARE);
                    return true;
                }
            }
        }
        //Si la direction est vers la gauche
        else if (dir==LEFT)
        {
            // On regarde si la case juste à gauche est walkable
            if(!isSquareWalkable(Coord::getDirPos(newPos,LEFT)))
            {
                //si non on regarde les cases adjacentes a la case visée
                if( !isSquareWalkable(Coord::getDirPos(newPos,TOP) ) || !isSquareWalkable(Coord::getDirPos(newPos,BOTTOM) ) )
                {
                    //Maze::setSquare(newPos, SPRITE_DEADSQUARE);
                    return true;
                }
            }
        }
        //Si la direction est vers la droite
        else if (dir==RIGHT)
        {
            // On regarde si la case juste à droite est walkable
            if(!isSquareWalkable(Coord::getDirPos(newPos,RIGHT)))
            {
                //si non on regarde les cases adjacentes a la case visée
                if( !isSquareWalkable(Coord::getDirPos(newPos,TOP) ) || !isSquareWalkable(Coord::getDirPos(newPos,BOTTOM) ) )
                {
                    //Maze::setSquare(newPos, SPRITE_DEADSQUARE);
                    return true;
                }
            }
        }
    }
    return false;
}//*/

//Permet de savoir si une case a une certaine position est une deadLock
bool Maze::isSquareDeadlock(short unsigned posBox, short unsigned dir, const std::vector<unsigned short>& posBoxes)
{
    //On calcule la position visée
    short unsigned newPos=Coord::getDirPos(posBox,dir);

    //On regarde si on ne vas pas sur un goal car un goal est un objectif donc pas une deadlock
    if(!isSquareGoal(newPos))
    {
        //Si la direction est vers le haut ou le bas
        if(dir==TOP || dir==BOTTOM)
        {
            // On regarde si la case juste devant est un mur
            if(isSquareWall(Coord::getDirPos(newPos,dir)))
            {
                //On stock les nouvelles coordonnées
                short unsigned newPosLeft=Coord::getDirPos(newPos,LEFT);
                short unsigned newPosRight=Coord::getDirPos(newPos,RIGHT);

                //si non on regarde si les cases adjacentes a la case visée sont des murs
                if(isSquareWall(newPosLeft) || isSquareWall( newPosRight) )
                {
                    //Maze::setSquare(newPos, SPRITE_DEADSQUARE);
                    return true;
                }
                //sinon on regarde si ce sont des caisses
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosLeft) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosLeft,dir))) return true;
                }
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosRight) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosRight,dir))) return true;
                }
            }
            //sinon on regarde si la case devant est une caisse
            else if( std::find(posBoxes.begin(), posBoxes.end(), Coord::getDirPos(newPos,dir)) != posBoxes.end() )
            {
                //On stock les nouvelles coordonnées
                short unsigned newPosLeft=Coord::getDirPos(newPos,LEFT);
                short unsigned newPosRight=Coord::getDirPos(newPos,RIGHT);

                //si non on regarde si les cases adjacentes a la case visée sont des murs
                if(isSquareWall(newPosLeft) || isSquareWall( newPosRight) )
                {
                    // alors il faut regarder si la box devant notre box sont walkable
                    if (!isSquareWalkable(Coord::getDirPos(newPosLeft,dir)) || !isSquareWalkable(Coord::getDirPos(newPosRight,dir)) )
                        return true;
                }
                //sinon on regarde si ce sont des caisses
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosLeft) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosLeft,dir))) return true;
                }
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosRight) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosRight,dir))) return true;
                }
            }
        }

        //Si la direction est vers le haut
        if(dir==LEFT || dir==RIGHT)
        {
            // On regarde si la case juste à coté  est walkable
            if(isSquareWall(Coord::getDirPos(newPos,dir)))
            {
                //On stock les nouvelles coordonnées
                short unsigned newPosTop=Coord::getDirPos(newPos,TOP);
                short unsigned newPosBottom=Coord::getDirPos(newPos,BOTTOM);

                //si non on regarde si les cases adjacentes a la case visée sont des murs
                if(isSquareWall(newPosTop) || isSquareWall( newPosBottom) )
                {
                    //Maze::setSquare(newPos, SPRITE_DEADSQUARE);
                    return true;
                }
                //sinon on regarde si ce sont des caisses
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosTop) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosTop,dir))) return true;
                }
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosBottom) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosBottom,dir))) return true;
                }
            }
            else if( std::find(posBoxes.begin(), posBoxes.end(), Coord::getDirPos(newPos,dir)) != posBoxes.end() )
            {
                //On stock les nouvelles coordonnées
                short unsigned newPosTop=Coord::getDirPos(newPos,LEFT);
                short unsigned newPosBottom=Coord::getDirPos(newPos,RIGHT);

                //si non on regarde si les cases adjacentes a la case visée sont des murs
                if(isSquareWall(newPosTop) || isSquareWall( newPosBottom) )
                {
                    // alors il faut regarder si la box devant notre box sont walkable
                    if (!isSquareWalkable(Coord::getDirPos(newPosTop,dir)) || !isSquareWalkable(Coord::getDirPos(newPosBottom,dir)) )
                        return true;
                }
                //sinon on regarde si ce sont des caisses
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosTop) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosTop,dir))) return true;
                }
                if( std::find(posBoxes.begin(), posBoxes.end(), newPosBottom) != posBoxes.end() )
                {
                    //si on trouve une caisse au regarde au dessus pour savoir si on peut pousser la caisse qu'on vient de croiser
                    if (!isSquareWalkable(Coord::getDirPos(newPosBottom,dir))) return true;
                }
            }
        }
    }
    return false;
}

//Permet de connaitre et d'afficher les deadLock a l'écran
void Maze::deadlocksDetectionActu()
{
    //Declaration et initialisation des variables
    unsigned short top=0, bottom=0, left=0, right=0;

    //On enleve les anciennes deadlock
    unsigned short numberCaseM=(this->m_lig*this->m_col);

    for(unsigned short i=0; i<numberCaseM; i++)
    {
        if(isSquareDead(i)) Maze::setSquare(i, SPRITE_GROUND);
    }

    //Pour chaque box de la matrice on regarde si les cases ajdacente sont walkable
    for(const auto& elem : getPosBoxes())
    {
        //Toutes les directions
        top=Coord::getDirPos(elem,TOP);
        bottom=Coord::getDirPos(elem,BOTTOM);
        right=Coord::getDirPos(elem,RIGHT);
        left=Coord::getDirPos(elem,LEFT);
        //Si la case n'est pas une goal
        if(!isSquareGoal(elem))
        {
            //On vérifier si la case est du sol
            //on regarde en haut
            if(isSquareGround(top))
            {
                if(!isSquareWalkable(Coord::getDirPos(top,TOP)))
                {
                    if( !isSquareWalkable(Coord::getDirPos(top,LEFT) ) || !isSquareWalkable(Coord::getDirPos(top,RIGHT) ) )
                        Maze::setSquare(top, SPRITE_DEADSQUARE);
                }
            }
            //on regarde en bas
            if(isSquareGround(bottom))
            {
                if(!isSquareWalkable(Coord::getDirPos(bottom,BOTTOM)))
                {
                    if( !isSquareWalkable(Coord::getDirPos(bottom,LEFT) ) || !isSquareWalkable(Coord::getDirPos(bottom,RIGHT) ) )
                        Maze::setSquare(bottom, SPRITE_DEADSQUARE);
                }
            }
            //On regarde a gauche
            if(isSquareGround(left))
            {
                if(!isSquareWalkable(Coord::getDirPos(left,LEFT)))
                {
                    if( !isSquareWalkable(Coord::getDirPos(left,BOTTOM) ) || !isSquareWalkable(Coord::getDirPos(left,TOP) ) )
                        Maze::setSquare(left, SPRITE_DEADSQUARE);
                }
            }
            //On regarde a droite
            if(isSquareGround(right))
            {
                if(!isSquareWalkable(Coord::getDirPos(right,RIGHT)))
                {
                    if( !isSquareWalkable(Coord::getDirPos(right,BOTTOM) ) || !isSquareWalkable(Coord::getDirPos(right,TOP) ) )
                        Maze::setSquare(right, SPRITE_DEADSQUARE);
                }
            }
        }
    }
}

//Sous programme qui retourne la position normalisé du joueur
unsigned short Maze::posNormalisee(std::unordered_set<short unsigned>& ZAP)
{
    //On délcare la position normalisé comem étant l'infini
    short unsigned posNorma=-1;
    //On parcours la ZAP pour trouver le plus petit nombre (qui correspond en fait a la case accessible le plus en HAUT a gauche)
    //Cette place nous permettra d'éviter les doublons dans nos noeud lorsque nous ferons nos algos
    for (const auto& elem: ZAP)
    {
        if (elem<posNorma) posNorma=elem;
    }
    return posNorma;
}

//Calculons la zone accessible du joueur
std::unordered_set<short unsigned> Maze::calculZAP(short unsigned posPlayer, const std::vector<unsigned short>& PosBoxes)
{
    //création et allocation de la file
    std::queue<short unsigned> _file;
    unsigned short postACtuelle, posTraitee=posPlayer;
    const char directions[4] = {TOP, BOTTOM, LEFT, RIGHT};
    bool caisse=false;

    ///Considérons une matrice de la taille du tableau initialisé à toutes ses cases à -1
    std::unordered_set<short unsigned> ZAP;

    ///Mettre un 1 à la position de mario == Marquage de la case de départ
    ZAP.emplace(posTraitee);
    //Enfilement de la case de départ
    _file.push(posTraitee);

    //Tant que la file n'est pas vide
    while(!_file.empty())
    {
        //Défiler la première case de la pile
        postACtuelle=_file.front();
        _file.pop();

        for (char dir: directions)
        {
            //On recupère la position a traiter
            posTraitee=Coord::getDirPos(postACtuelle,dir);

            //On regarde si la pos a traiter n'est pas déja marquée
            caisse=(std::find(PosBoxes.begin(), PosBoxes.end(), posTraitee)!=PosBoxes.end());

            //Si le déplacement dans la direction donnée est possible
            if ( !isSquareWall(posTraitee) && caisse==false && ZAP.find(posTraitee) == ZAP.end())
            {
                //On marque la positon comme possible
                ZAP.emplace(posTraitee);
                //On enfile la position
                _file.push(posTraitee);
                ///Maze::setSquare(posTraitee, SPRITE_ZAP);
            }
        }
    }
    ///std::cout << this->posNormalisee(ZAP) << std::endl;
    return ZAP;
}

bool Maze::canIPushABoxFromAccessibleZonePlayerInADirection(unsigned short posBox, std::unordered_set<unsigned short>& ZAP, char dir, const std::vector<unsigned short>& posBoxes)
{
    unsigned short posAimedBox = Coord::getDirPos(posBox,dir);
    unsigned short posPlayerBeforePush = Coord::getOppositeDirPos(posBox,dir);
    if(m_actifDeadlock)
        return (!isSquareWall(posAimedBox) // la position d'arrivé n'est pas un mur ?
            && ZAP.find(posPlayerBeforePush)!=ZAP.end() // la zone est elle accessible par le petit Mario
            && std::find(posBoxes.begin(), posBoxes.end(), posAimedBox) == posBoxes.end() //Est ce qu'il y a caisse derrier la caise
            && !isSquareDeadlock(posBox, dir, posBoxes)); // si la case est une deadlock alors non

    return (!isSquareWall(posAimedBox) // la position d'arrivé n'est pas un mur ?
            && ZAP.find(posPlayerBeforePush)!=ZAP.end() // la zone est elle accessible par le petit Mario
            && std::find(posBoxes.begin(), posBoxes.end(), posAimedBox) == posBoxes.end()
            && !isSquareDead(posAimedBox)); //Est ce qu'il y a caisse derrier la caise
}

///Algo général du solveur BFS:
void Maze::solvBFS()
{
    //Pour mesurer le temps
    clock_t start;
    clock_t time;
    start = std::clock();
    int cpt=0;

    //Déclartion des variables
    Noeud* NoeudCourant = new Noeud(NULL, getPosBoxes(), -1, 1, -1)/*, *NoeudAdjacent*/;
    Noeud* NoeudVict = new Noeud(NULL, getPosBoxes(), -1, 1, -1);
    bool win=false;
    const char directions[4] = {TOP, BOTTOM, LEFT, RIGHT};
    std::vector<unsigned short> newPosBoxes = getPosBoxes();
    std::unordered_map<size_t, Noeud*> myMap;
    size_t sizeHachage;
    unsigned short nbBoxes, posBox;

    //Déclaration de la file
    std::queue<Noeud*> fileNoeud;

    // Calculer la zone accessible du player (ZAP) et récupérer la position normalisée
    std::unordered_set<short unsigned> ZAPChild;
    std::unordered_set<short unsigned> ZAPParent=calculZAP(getPosPlayer(), newPosBoxes);
    unsigned short posNorma=posNormalisee(ZAPParent);

    // Créer un  nœud Initial
    Noeud *n=new Noeud(NULL, getPosBoxes(), 0, posNorma, 0);

    //Marque
    myMap.emplace(n->hashCode(), n);

    // Enfiler le nœud initial
    fileNoeud.push(n);
    nbBoxes=NoeudCourant->getNoeudRefPosBoxes().size();

    // Tant que la file n’est pas vide
    while(!fileNoeud.empty() && !win)
    {
        cpt++;
        // Défiler le nom courant : courant (Nœud*)
        NoeudCourant=fileNoeud.front();
        fileNoeud.pop();

        //On vérifie si l'état est solution
        //Est ce que toutes les caisses sont sur un goal ?
        if (std::is_permutation(getGoals().begin(), getGoals().end(),NoeudCourant->getNoeudPosBoxes().begin()))
        {
            win =true;
            NoeudVict=NoeudCourant;
            break;
        }

        // Calculer ZAP parent + position normalisée
        ZAPParent=calculZAP(NoeudCourant->getPosNormMario(), NoeudCourant->getNoeudRefPosBoxes());

        // Pour chaque caisse du Noeud courant (i)
        for(unsigned short i=0; i<nbBoxes; i++)
        {
            //On stock la position de la box
            posBox=NoeudCourant->getNoeudRefPosBoxes()[i];

            // On associe aux caisses leur ancienne position avant d'en déplacer une
            newPosBoxes=NoeudCourant->getNoeudPosBoxes();

            // Pour chaque direction (Haut bas gauche droite) (j)
            for (char dir: directions)
            {
                ///pour chaque caisse accessible grace a la zap et // Puis-je pousser la caisse de « i » dans la direction « j »
                if (canIPushABoxFromAccessibleZonePlayerInADirection(posBox, ZAPParent, dir, NoeudCourant->getNoeudRefPosBoxes()))
                {
                    // On actualise la position de la caisse poussé dans le vector
                    newPosBoxes[i]=Coord::getDirPos(posBox,dir);

                    // Recalculer la nouvelle ZAP enfant et la position normalisée
                    ZAPChild=calculZAP(posBox, newPosBoxes); ///ZAPChild=calculZAP(newPosBoxes[i], newPosBoxes);
                    posNorma=posNormalisee(ZAPChild);

                    // Créer un nouvel état (Nœud*) : newStade
                    Noeud* newStade=new Noeud(NULL, newPosBoxes, dir, posNorma, i);
                    sizeHachage=newStade->hashCode();

                    // Si l’état n’est pas un doublon (table de Hachage)
                    if(myMap.find (sizeHachage)==myMap.end())
                    {
                        newStade->setNparent(NoeudCourant);
                        // Enfiler newStade
                        fileNoeud.push(newStade);
                        /// std::cout << newStade->getNParent()->getDirBoxPush() << " PB:" << newStade->getNoeudPosBoxes()[1] << " PM:" << newStade->getPosNormMario() << " DIR"<< newStade->getDirBoxPush() << " IND" << newStade->getIndexBosPush() << std::endl;
                        //Ajouter A la table de Hachage
                        myMap.emplace(sizeHachage, newStade);
                    }
                    else delete newStade;
                }
            }
        }
    }
    /*int i =0;
    while (NoeudCourant!=NULL && i <100) {
        for (auto i:m_pos_boxes) setSquare(i,SPRITE_GROUND);
        for (auto i:m_pos_goals) setSquare(i,SPRITE_GOAL);
        for (auto i:NoeudCourant->getNoeudPosBoxes()) setSquare(i,SPRITE_BOX);
        m_pos_boxes = NoeudCourant->getNoeudPosBoxes();
        std::cout <<(*this)<<std::endl;
        NoeudCourant = NoeudCourant->getNParent();
        i++;
    }*/
    time=std::clock();
    int sec=(time-start)/1000;
    int cent=(time-start)/10;

    if(win)
    {
        std::cout << "You Win ! ... NOOOO ! The computer Win !" <<std::endl;
        visualisationWin(NoeudVict);

        //affichage du temps
        std::cout <<"Find in "<< sec <<","<< cent-sec*100 <<" secondes"<<std::endl;
        //affichage du nombre de noeuds traitées
        std::cout << cpt <<" node processed"<<std::endl;
    }

    std::unordered_map<size_t,Noeud*>::iterator it;
    for(it=myMap.begin(); it!=myMap.end();)
    {
        delete it->second;
        it=myMap.erase(it);
    }
}

void Maze::solvDFS()
{
    //Pour mesurer le temps
    clock_t start;
    clock_t time;
    start = std::clock();
    int cpt=0;

    //Déclartion des variables
    Noeud* NoeudCourant = new Noeud(NULL, getPosBoxes(), -1, 1, -1)/*, *NoeudAdjacent*/;
    Noeud* NoeudVict = new Noeud(NULL, getPosBoxes(), -1, 1, -1);
    bool win=false;
    const char directions[4] = {TOP, BOTTOM, LEFT, RIGHT};
    std::vector<unsigned short> newPosBoxes = getPosBoxes();
    std::unordered_map<size_t, Noeud*> myMap;
    size_t sizeHachage;
    unsigned short nbBoxes, posBox;

    //Déclaration de la pile
    std::stack<Noeud*> pileNoeud;

    // Calculer la zone accessible du player (ZAP) et récupérer la position normalisée
    std::unordered_set<short unsigned> ZAPChild;
    std::unordered_set<short unsigned> ZAPParent=calculZAP(getPosPlayer(), newPosBoxes);
    unsigned short posNorma=posNormalisee(ZAPParent);

    // Créer un  nœud Initial
    Noeud *n=new Noeud(NULL, getPosBoxes(), 0, posNorma, 0);

    //Marque
    myMap.emplace(n->hashCode(), n);

    // Enfiler le nœud initial
    pileNoeud.push(n);
    nbBoxes=NoeudCourant->getNoeudRefPosBoxes().size();

    // Tant que la pile n’est pas vide
    while(!pileNoeud.empty() && !win)
    {
        cpt++;
        // Défiler le nom courant : courant (Nœud*)
        NoeudCourant=pileNoeud.top();
        pileNoeud.pop();

        //On vérifie si l'état est solution
        //Est ce que toutes les caisses sont sur un goal ?
        if (std::is_permutation(getGoals().begin(), getGoals().end(),NoeudCourant->getNoeudPosBoxes().begin()))
        {
            win =true;
            NoeudVict=NoeudCourant;
            break;
        }

        // Calculer ZAP parent + position normalisée
        ZAPParent=calculZAP(NoeudCourant->getPosNormMario(), NoeudCourant->getNoeudRefPosBoxes());

        // Pour chaque caisse du Noeud courant (i)
        for(unsigned short i=0; i<nbBoxes; i++)
        {
            //On stock la position de la box
            posBox=NoeudCourant->getNoeudRefPosBoxes()[i];

            // On associe aux caisses leur ancienne position avant d'en déplacer une
            newPosBoxes=NoeudCourant->getNoeudPosBoxes();

            // Pour chaque direction (Haut bas gauche droite) (j)
            for (char dir: directions)
            {
                ///pour chaque caisse accessible grace a la zap et // Puis-je pousser la caisse de « i » dans la direction « j »
                if (canIPushABoxFromAccessibleZonePlayerInADirection(posBox, ZAPParent, dir, NoeudCourant->getNoeudRefPosBoxes()))
                {
                    // On actualise la position de la caisse poussé dans le vector
                    newPosBoxes[i]=Coord::getDirPos(posBox,dir);

                    // Recalculer la nouvelle ZAP enfant et la position normalisée
                    ZAPChild=calculZAP(posBox, newPosBoxes); ///ZAPChild=calculZAP(newPosBoxes[i], newPosBoxes);
                    posNorma=posNormalisee(ZAPChild);

                    // Créer un nouvel état (Nœud*) : newStade
                    Noeud* newStade=new Noeud(NoeudCourant, newPosBoxes, dir, posNorma, i);
                    sizeHachage=newStade->hashCode();

                    // Si l’état n’est pas un doublon (table de Hachage)
                    if(myMap.find (sizeHachage)==myMap.end())
                    {
                        // Enfiler newStade
                        pileNoeud.push(newStade);
                        /// std::cout << newStade->getNParent()->getDirBoxPush() << " PB:" << newStade->getNoeudPosBoxes()[1] << " PM:" << newStade->getPosNormMario() << " DIR"<< newStade->getDirBoxPush() << " IND" << newStade->getIndexBosPush() << std::endl;
                        //Ajouter A la table de Hachage
                        myMap.emplace(sizeHachage, newStade);
                    }
                    else
                    {
                        delete newStade;
                    }
                }
            }
        }
    }
     /*int i =0;
     while (NoeudCourant!=NULL && i <100) {
         for (auto i:m_pos_boxes) setSquare(i,SPRITE_GROUND);
         for (auto i:m_pos_goals) setSquare(i,SPRITE_GOAL);
         for (auto i:NoeudCourant->getNoeudPosBoxes()) setSquare(i,SPRITE_BOX);
         m_pos_boxes = NoeudCourant->getNoeudPosBoxes();
         std::cout <<(*this)<<std::endl;
         NoeudCourant = NoeudCourant->getNParent();
         i++;
     }//*/
    time=std::clock();
    int sec=(time-start)/1000;
    int cent=(time-start)/10;

    if(win)
    {
        std::cout << "You Win ! ... NOOOO ! The computer Win !" <<std::endl;
        visualisationWin(NoeudVict);

        //affichage du temps
        std::cout <<"Find in "<< sec <<","<< cent-sec*100 <<" secondes"<<std::endl;
        //affichage du nombre de noeuds traitées
        std::cout << cpt <<" node processed"<<std::endl;
    }

    std::unordered_map<size_t,Noeud*>::iterator it;
    for(it=myMap.begin(); it!=myMap.end();)
    {
        delete it->second;
        it=myMap.erase(it);
    }

    if(win) std::cout << "WIN" <<std::endl;
}

std::vector<char> Maze::Bruteforce(bool win, std::vector<char>& direct, Graphic g)
{
    int last=0;
    direct.push_back(TOP);
    int cpt=1;
    std::vector<unsigned char> save_field=m_field;
    std::vector<unsigned short> save_pos_boxes=m_pos_boxes;
    unsigned short save_pos_player=m_pos_player;

    while (cpt<1000000000&& !win && !g.keyGet(KEY_ESC))
    {
        if (cpt > 0)
        {
            for (int i=direct.size()+1; i>=0; i--)
            {
                win=updatePlayer(direct[i]);
                if (win)
                {
                    std::cout << "Win ! " << std::endl;
                    cpt=-1;
                    i=-1;
                }
            }
        }
        if (cpt>0)
        {
            if (!win)
            {
                m_pos_boxes=save_pos_boxes;
                m_pos_player=save_pos_player;
                m_field=save_field;
            }

            cpt++;

            if (cpt>0)
            {
                last =direct.size()-1;
                direct[last]++;
                while(direct[last]==MAX_DIR)
                {
                    direct[last]=0;
                    if(last>0)
                    {
                        direct[last-1]++;
                        last--;
                    }
                    else
                    {
                        direct.push_back(0);
                    }
                }
            }
        }
        ///std::cout<<cpt<<std::endl;
    }
    if(!win)
        std::cout<<"echec"<<std::endl;
    m_pos_boxes=save_pos_boxes;
    m_pos_player=save_pos_player;
    m_field=save_field;
    return direct;
}

unsigned short Maze::BFSplayer(Noeud *pred, Noeud *succ, unsigned short posMario)
{
    //création et allocation de la file
    std::queue<unsigned short> _file;

    //Tableau de position avec d'un predecesseur qui fait aussi tableau de marquage
    std::unordered_map<unsigned short, unsigned short> predecesseurs;

    //On récupère la position juste avant la poussé de la caisse
    unsigned short arrival=Coord::getOppositeDirPos(pred->getNoeudPosBoxes()[succ->getIndexBosPush()],succ->getDirBoxPush());
    const char directions[4] = {TOP, BOTTOM, LEFT, RIGHT};
    unsigned short posActu=arrival, posTraitee;

    //On crée un booléen pour pas que le BFS ne calcul plus loin que la ZAP
    bool caisse=false;
    std::vector<unsigned short> PosBoxes=pred->getNoeudPosBoxes();

    //Marquage de la case de départ
    predecesseurs[arrival]=arrival;

    //Enfilement de la case de départ
    _file.push(arrival);

    //Tant que la file n'est pas vide
    while(!_file.empty() && posActu!=posMario)
    {
        //Défiler la première case de la pile
        posActu=_file.front();
        _file.pop();
        for (char dir: directions)
        {
            //On recupère la position a traiter
            posTraitee=Coord::getDirPos(posActu,dir);

            //On regarde si la pos a traiter n'est pas une caisse et donc ne bloque pas le passage
            caisse=(std::find(PosBoxes.begin(), PosBoxes.end(), posTraitee)==PosBoxes.end());

            //Si le déplacement dans la direction donnée est possible
            if ( !isSquareWall(posTraitee) && caisse && predecesseurs.find(posTraitee) == predecesseurs.end())
            {
                //On enfile la position
                _file.push(posTraitee);
                //On marque son prédecesseur
                predecesseurs[posTraitee]=posActu;
            }
        }
    }
    if (posActu==posMario)
    {
        while(posActu!=arrival)
        {
            getdirMario().push(Coord::getPosDir(predecesseurs[posActu]-posActu));
            posActu=predecesseurs[posActu];
        }
    }
    else std::cout << " Dommage " << std::endl;
    getdirMario().push(succ->getDirBoxPush());
    return (Coord::getDirPos(arrival,succ->getDirBoxPush()));
}

void Maze::displayPath(Graphic g, unsigned short msec)
{
    while(!getdirMario().empty() && !g.keyGet(KEY_ESC))
    {
        updatePlayer(getdirMario().front());
        getdirMario().pop();
        sleep(msec);
        g.clear();
        draw(g);
        g.display(Coord::m_nb_col);
    }
}

void Maze::visualisationWin(Noeud *lastNoeud)
{
    std::stack<Noeud*> pileNoeud;
    unsigned short posMario=getPosPlayer();
    while (lastNoeud!=NULL)
    {
        pileNoeud.push(lastNoeud);
        lastNoeud = lastNoeud->getNParent();
    }
    Noeud* pred=pileNoeud.top();
    pileNoeud.pop();
    Noeud* succ=pileNoeud.top();
    pileNoeud.pop();
    while(!pileNoeud.empty())
    {
            /*for (auto i:m_pos_boxes) setSquare(i,SPRITE_GROUND);
            for (auto i:m_pos_goals) setSquare(i,SPRITE_GOAL);
            for (auto i:pred->getNoeudPosBoxes()) setSquare(i,SPRITE_BOX);
            m_pos_boxes = pred->getNoeudPosBoxes();
            std::cout <<(*this)<<std::endl;*/
        posMario=BFSplayer(pred, succ, posMario);
        pred=succ;
        succ=pileNoeud.top();
        pileNoeud.pop();
    }
    posMario=BFSplayer(pred, succ, posMario);
}


void Maze::deadlocksStatic()
{
    //decalration des variables
    char dir = TOP;
    unsigned int line,col;
    unsigned short PosAct;
    bool square_gnd=true;
    std::vector<unsigned short> vDeadLock;
    unsigned short sizeOfVDL;
    //Zapp de deadlock avec aucune case
    std::unordered_set <short unsigned> ZAP=calculZAP(this->getPosPlayer(), vDeadLock);
    // on fait -1 pour ne pas dépasser de la matrice
    for (unsigned int i = m_col+1; i < m_field.size()-m_col-1; i++)
    {
        //avec la ZAP on accès à tous ce qui est intérieur
        if (((m_field[i-m_col] == SPRITE_WALL && m_field[i-1] == SPRITE_WALL)
                || (m_field[i-m_col] == SPRITE_WALL && m_field[i+1] == SPRITE_WALL)
                || (m_field[i+m_col] == SPRITE_WALL && m_field[i+1] == SPRITE_WALL)
                || (m_field[i+m_col] == SPRITE_WALL && m_field[i-1] == SPRITE_WALL))
                && (m_field[i]==SPRITE_GROUND||m_field[i]==SPRITE_MARIO)
                && ZAP.find(i)!= ZAP.end())
        {
            m_field[i] = SPRITE_DEADSQUARE;
            vDeadLock.push_back(i);
        }
    }
    sizeOfVDL=vDeadLock.size();
    if (sizeOfVDL>0)
    {
        for (unsigned int i=0; i<sizeOfVDL-1; i++)
        {
            dir=TOP;
            for(int j=0; j<MAX_DIR+1; j++)
            {
                square_gnd=true;
                PosAct=vDeadLock[i];
                Coord::coord2D(PosAct,line,col);
                if (line>0)
                {
                    if((dir==TOP)&&(isSquareWall(Coord::getDirPos(vDeadLock[i],LEFT))))
                    {
                        while(isSquareWall(Coord::getDirPos(PosAct,TOP))&&!isSquareWall(Coord::getDirPos(PosAct,RIGHT)))
                        {
                            PosAct=Coord::getDirPos(PosAct,RIGHT);
                            GroundLine(PosAct,square_gnd);
                        }
                        if(isSquareDead(PosAct)&&square_gnd)
                        {
                            while(!isSquareWall(Coord::getDirPos(PosAct,LEFT)))
                            {
                                PosAct=Coord::getDirPos(PosAct,LEFT);
                                if(isSquareGround(PosAct))
                                {
                                    m_field[PosAct]=SPRITE_DEADSQUARE;
                                }
                            }
                        }
                    }

                    if((dir==BOTTOM)&&(isSquareWall(Coord::getDirPos(vDeadLock[i],LEFT))))
                    {

                        PosAct=vDeadLock[i];


                        while(isSquareWall(Coord::getDirPos(PosAct,BOTTOM))&&!isSquareWall(Coord::getDirPos(PosAct,RIGHT)))
                        {
                            PosAct=Coord::getDirPos(PosAct,RIGHT);
                            GroundLine(PosAct,square_gnd);
                        }
                        if(isSquareDead(PosAct)&&square_gnd)
                        {
                            while(!isSquareWall(Coord::getDirPos(PosAct,LEFT)))
                            {
                                PosAct=Coord::getDirPos(PosAct,LEFT);
                                if(isSquareGround(PosAct))
                                {
                                    m_field[PosAct]=SPRITE_DEADSQUARE;
                                }
                            }
                        }
                    }

                    if((dir==LEFT)&&(isSquareWall(Coord::getDirPos(vDeadLock[i],TOP))))
                    {
                        PosAct=vDeadLock[i];

                        while(isSquareWall(Coord::getDirPos(PosAct,LEFT))&&!isSquareWall(Coord::getDirPos(PosAct,BOTTOM)))
                        {
                            PosAct=Coord::getDirPos(PosAct,BOTTOM);
                            GroundLine(PosAct,square_gnd);
                        }
                        if(isSquareDead(PosAct)&&square_gnd)
                        {
                            while(!isSquareWall(Coord::getDirPos(PosAct,TOP)))
                            {
                                PosAct=Coord::getDirPos(PosAct,TOP);
                                if(isSquareGround(PosAct))
                                {
                                    m_field[PosAct]=SPRITE_DEADSQUARE;
                                }
                            }
                        }
                    }

                    if((dir==RIGHT)&&(isSquareWall(Coord::getDirPos(vDeadLock[i],TOP))))
                    {
                        PosAct=vDeadLock[i];

                        while(isSquareWall(Coord::getDirPos(PosAct,RIGHT))&&!isSquareWall(Coord::getDirPos(PosAct,BOTTOM)))
                        {
                            PosAct=Coord::getDirPos(PosAct,BOTTOM);
                            GroundLine(PosAct,square_gnd);
                        }
                        if(isSquareDead(PosAct)&&square_gnd)
                        {
                            while(!isSquareWall(Coord::getDirPos(PosAct,TOP)))
                            {
                                PosAct=Coord::getDirPos(PosAct,TOP);
                                if(isSquareGround(PosAct))
                                {
                                    m_field[PosAct]=SPRITE_DEADSQUARE;
                                }
                            }
                        }
                    }
                }
                dir++;
            }
        }
    }
}

void Maze::GroundLine( unsigned short position,bool& ground)
{
    if((isSquareGoal(position))||(isSquareBoxPlaced(position))||isSquareWall(position))
    {
        ground=false;
    }
}

void Maze::solvIDA()
{
    solvIDA(true);
}

void Maze::solvBestFS()
{
    solvIDA(false);
}

void Maze::solvIDA(bool isAStar)
{
    //Pour mesurer le temps
    clock_t start;
    clock_t time;
    start = std::clock();
    int cpt=0;

    //Déclartion des variables
    Noeud* NoeudCourant = new Noeud(NULL, getPosBoxes(), -1, 1, -1, -1,isAStar)/*, *NoeudAdjacent*/;
    Noeud* NoeudVict = new Noeud(NULL, getPosBoxes(), -1, 1, -1, -1, isAStar);

    bool win=false;
    const char directions[4] = {TOP, BOTTOM, LEFT, RIGHT};
    size_t sizeHachage;
    unsigned short nbBoxes, posBox;

    std::vector<unsigned short> newPosBoxes = getPosBoxes();
    std::unordered_map<size_t, Noeud*> myMap;

    //Déclaration de la file
    std::priority_queue<Noeud*, std::vector<Noeud*>, heuristicCompare> fileNoeud;

    // Calculer la zone accessible du player (ZAP) et récupérer la position normalisée
    std::unordered_set<short unsigned> ZAPChild;
    std::unordered_set<short unsigned> ZAPParent=calculZAP(getPosPlayer(), newPosBoxes);
    unsigned short posNorma=posNormalisee(ZAPParent);

    // Créer un  nœud Initial
    Noeud *n=new Noeud(NULL, getPosBoxes(), 0, posNorma, 0, -1, isAStar);
    n->setHCost(heuristic(n));
    //Marque
    myMap.emplace(n->hashCode(), n);

    // Enfiler le nœud initial
    fileNoeud.push(n);
    nbBoxes=NoeudCourant->getNoeudRefPosBoxes().size();

    // Tant que la file n’est pas vide
    while(!fileNoeud.empty() && !win)
    {
        cpt++;
        // Défiler le nom courant : courant (Nœud*)
        NoeudCourant=fileNoeud.top();
        ///std::cout << NoeudCourant->getCost()<<std::endl;
        fileNoeud.pop();

        //On vérifie si l'état est solution
        //Est ce que toutes les caisses sont sur un goal ?
        if (std::is_permutation(getGoals().begin(), getGoals().end(),NoeudCourant->getNoeudPosBoxes().begin()))
        {
            win =true;
            NoeudVict=NoeudCourant;
            break;
        }

        // Calculer ZAP parent + position normalisée
        ZAPParent=calculZAP(NoeudCourant->getPosNormMario(), NoeudCourant->getNoeudRefPosBoxes());

        // Pour chaque caisse du Noeud courant (i)
        for(unsigned short i=0; i<nbBoxes; i++)
        {
            //On stock la position de la box
            posBox=NoeudCourant->getNoeudRefPosBoxes()[i];

            // On associe aux caisses leur ancienne position avant d'en déplacer une
            newPosBoxes=NoeudCourant->getNoeudPosBoxes();

            // Pour chaque direction (Haut bas gauche droite) (j)
            for (char dir: directions)
            {
                ///pour chaque caisse accessible grace a la zap et // Puis-je pousser la caisse de « i » dans la direction « j »
                if (canIPushABoxFromAccessibleZonePlayerInADirection(posBox, ZAPParent, dir, NoeudCourant->getNoeudRefPosBoxes()))
                {
                    // On actualise la position de la caisse poussé dans le vector
                    newPosBoxes[i]=Coord::getDirPos(posBox,dir);

                    // Recalculer la nouvelle ZAP enfant et la position normalisée
                    ZAPChild=calculZAP(posBox, newPosBoxes);
                    posNorma=posNormalisee(ZAPChild);

                    // Créer un nouvel état (Nœud*) : newStade
                    Noeud* newStade=new Noeud(NULL, newPosBoxes, dir, posNorma, i, -1, isAStar);
                    sizeHachage=newStade->hashCode();

                    // Si l’état n’est pas un doublon (table de Hachage)

                    if(myMap.find (sizeHachage)==myMap.end())
                    {
                        NoeudCourant->setHCost(heuristic(NoeudCourant));
                        newStade->setNparent(NoeudCourant);
                        // Enfiler newStade
                        fileNoeud.push(newStade);
                        //Ajouter A la table de Hachage
                        myMap.emplace(sizeHachage, newStade);
                    }
                    else delete newStade;
                }
            }
        }
    }

    time=std::clock();
    int sec=(time-start)/1000;
    int cent=(time-start)/10;

    if(win)
    {
        std::cout << "You Win ! ... NOOOO ! The computer Win !" <<std::endl;
        visualisationWin(NoeudVict);

        //affichage du temps
        std::cout <<"Find in "<< sec <<","<< cent-sec*100 <<" secondes"<<std::endl;
        //affichage du nombre de noeuds traitées
        std::cout << cpt <<" node processed"<<std::endl;
    }

    std::unordered_map<size_t,Noeud*>::iterator it;
    for(it=myMap.begin(); it!=myMap.end();)
    {
        delete it->second;
        it=myMap.erase(it);
    }

    if(win) std::cout << "WIN" <<std::endl;
}

unsigned int Maze::heuristic(Noeud* noeud)
{
    unsigned int heuristicCost = 0;
    unsigned short nbGoals=getGoals().size();
    ///bool isTaken[nbGoals] = {false};

    for(int i=0; i<nbGoals; i++)
    {
        unsigned int lGoal, cGoal, minimum;
        bool firstLoop = true;
        Coord::coord2D(getGoals()[i], lGoal, cGoal);

        for(int j=0; j<nbGoals; j++)
        {
            unsigned int lBox, cBox, minimumTemp;
            Coord::coord2D(noeud->getNoeudPosBoxes()[i], lBox, cBox);

            if(lGoal>lBox) minimumTemp = lGoal - lBox;
            else minimumTemp = lBox - lGoal;

            if(cGoal>cBox) minimumTemp += cGoal - cBox;
            else minimumTemp += cBox - cGoal;

            if(minimumTemp<minimum || firstLoop)
            {
                firstLoop = false;
                minimum = minimumTemp;
            }
        }
        heuristicCost +=minimum;
    }
   /* unsigned int heuristicCost = 0;
    unsigned short nbGoals=getGoals().size();
    bool isTaken[nbGoals] = {false};

    for(int i=0; i<nbGoals; i++)
    {

        unsigned int lbox, cbox;
        Coord::coord2D(noeud->getNoeudPosBoxes()[i], lbox, cbox);
        unsigned int tempCost = -1, lowerBound = -1 ;

        for(int j=0; j<nbGoals; j++)
        {
            if(!isTaken[j])
            {
                unsigned int lgoal, cgoal, temp = 0;
                Coord::coord2D(getGoals()[j], lgoal, cgoal);

                if(lgoal>lbox) temp += lgoal - lbox;
                else temp += lbox - lgoal;

                if(cgoal>cbox) temp += cgoal - cbox;
                else temp += cbox - cgoal;

                if (temp < tempCost)
                {
                    lowerBound = j;
                    tempCost = temp;
                }
            }
        }
        heuristicCost += tempCost;
        isTaken[lowerBound] = true;
    }*/
    return heuristicCost;
}

void Maze::pBestFS()
{
    //Pour mesurer le temps
    clock_t start;
    clock_t time;
    start = std::clock();
    int cpt=0;

    bool win=false;

    //On recupere la position des boxes et leur nombres
    std::vector<unsigned short> posBoxes = m_pos_boxes;
    unsigned short nbBox=m_pos_boxes.size();
    unsigned short posBoxActu;

    //La file
    std::queue<Noeud*> _FileBestFS;

    //Map de marquage
    std::vector<Noeud*> marquage;
    std::unordered_map<size_t, Noeud*> myMap;
    size_t sizeHachage;

    // ZAP et pos Normalisé
    std::unordered_set<short unsigned> ZAPparent=calculZAP(m_pos_player, posBoxes), ZAPenfant;
    unsigned short posNormMario=posNormalisee(ZAPparent);

    // On initialise les differents noeuds utilisé plus tard
    Noeud* NoeudCourant = new Noeud(NULL, m_pos_boxes, TOP, posNormMario,0);
    Noeud* NoeudVict = new Noeud(NULL, m_pos_boxes, TOP, 1,0);
    Noeud* noeud_TOP = new Noeud(NULL, m_pos_boxes, TOP, 1,0);
    Noeud* noeud_BOTTOM = new Noeud(NULL, m_pos_boxes, TOP, 1,0);
    Noeud* noeud_LEFT = new Noeud(NULL, m_pos_boxes, TOP, 1,0);
    Noeud* noeud_RIGHT = new Noeud(NULL, m_pos_boxes, TOP, 1,0);

    //On le push et marque
    marquage.push_back(NoeudCourant);
    _FileBestFS.push(NoeudCourant);

    //Pour le BestFS, regarder le poids des direction
    unsigned short p_TOP=0;
    unsigned short p_BOTTOM=0;
    unsigned short p_LEFT=0;
    unsigned short p_RIGHT=0;
    unsigned int lineBox,ColBox,LinGoal,ColGoal;
    bool top_pushed=false;
    bool bot_pushed=false;
    bool left_pushed=false;
    bool right_pushed=false;
    bool GoOnTop=false;
    bool GoOnBottom=false;
    bool GoOnRight=false;
    bool GoOnLeft=false;
    // Debut BestfS
    while(!_FileBestFS.empty() && !win)
    {
        //Compteur de noeud traité
        cpt++;

        NoeudCourant=_FileBestFS.front();
        _FileBestFS.pop();

        //On verifie si on a gagné
        if (std::is_permutation(getGoals().begin(), getGoals().end(),NoeudCourant->getNoeudPosBoxes().begin()))
        {
            win =true;
            NoeudVict=NoeudCourant;
            break;
        }
        // si c'est le cas
        if(!win){
            //On calcul la ZAP du noeud courant
            ZAPparent=calculZAP(NoeudCourant->getPosNormMario(), NoeudCourant->getNoeudRefPosBoxes());

            //On déplace toutes les boxes du niveau donc i boxes
            for(unsigned short i=0; i<nbBox; i++)
            {
                //On sauvegarde la position de la box traitée
                posBoxActu=NoeudCourant->getNoeudPosBoxes()[i];
                GoOnTop=false;
                GoOnBottom=false;
                GoOnRight=false;
                GoOnLeft=false;

                //On sauvegarde la position des caisses
                posBoxes=NoeudCourant->getNoeudPosBoxes();
                std::vector<int>RankingNode;

                if (canIPushABoxFromAccessibleZonePlayerInADirection(posBoxActu, ZAPparent, TOP, NoeudCourant->getNoeudPosBoxes()))
                {
                    //Nouvelle position de la box séléctionné
                    posBoxes[i]=Coord::getDirPos(posBoxActu,TOP);

                    //on calcul la ZAP et la pos Normale
                    ZAPenfant=calculZAP(posBoxActu, posBoxes);
                    posNormMario=posNormalisee(ZAPenfant);

                    //On crée un noeud adapté
                    noeud_TOP=new Noeud(NULL, posBoxes, TOP, posNormMario,i);
                    Coord::coord2D(m_pos_boxes[i],lineBox,ColBox);
                    Coord::coord2D(m_pos_goals[i],LinGoal,ColGoal);
                    p_TOP=(LinGoal-lineBox-1)+(ColGoal-ColBox);

                    sizeHachage=noeud_TOP->hashCode();
                    // Si l’état n’est pas un doublon (table de Hachage)
                    if(myMap.find (sizeHachage)==myMap.end())
                    {
                        noeud_TOP->setNparent(NoeudCourant);
                        //Ajouter A la table de Hachage
                        myMap.emplace(sizeHachage, noeud_TOP);
                        //Lui ajouter une note #MasterChef
                        RankingNode.push_back(p_TOP);
                        GoOnTop=true;
                    }
                }

                if (canIPushABoxFromAccessibleZonePlayerInADirection(posBoxActu, ZAPparent, BOTTOM, NoeudCourant->getNoeudPosBoxes()))
                {

                    //Nouvelle position de la box séléctionné
                    posBoxes[i]=Coord::getDirPos(posBoxActu,BOTTOM);

                    //on calcul la ZAP et la pos Normale
                    ZAPenfant=calculZAP(posBoxActu, posBoxes);
                    posNormMario=posNormalisee(ZAPenfant);

                    //On crée un noeud adapté
                    noeud_BOTTOM=new Noeud(NULL, posBoxes, BOTTOM, posNormMario,i);
                    Coord::coord2D(m_pos_boxes[i],lineBox,ColBox);
                    Coord::coord2D(m_pos_goals[i],LinGoal,ColGoal);
                    p_BOTTOM=(LinGoal-lineBox+1)+(ColGoal-ColBox);

                    sizeHachage=noeud_BOTTOM->hashCode();
                    // Si l’état n’est pas un doublon (table de Hachage)
                    if(myMap.find (sizeHachage)==myMap.end())
                    {
                        noeud_BOTTOM->setNparent(NoeudCourant);
                        //Ajouter A la table de Hachage
                        myMap.emplace(sizeHachage, noeud_BOTTOM);
                        //Lui ajouter une note #MasterChef
                        RankingNode.push_back(p_BOTTOM);
                        GoOnBottom=true;
                    }
                }

                if (canIPushABoxFromAccessibleZonePlayerInADirection(posBoxActu, ZAPparent, LEFT, NoeudCourant->getNoeudPosBoxes()))
                {

                    //Nouvelle position de la box séléctionné
                    posBoxes[i]=Coord::getDirPos(posBoxActu,LEFT);

                    //on calcul la ZAP et la pos Normale
                    ZAPenfant=calculZAP(posBoxActu, posBoxes);
                    posNormMario=posNormalisee(ZAPenfant);

                    //On crée un noeud adapté
                    noeud_LEFT=new Noeud(NULL, posBoxes, LEFT, posNormMario,i);
                    Coord::coord2D(m_pos_boxes[i],lineBox,ColBox);
                    Coord::coord2D(m_pos_goals[i],LinGoal,ColGoal);
                    p_LEFT=(LinGoal-lineBox)+(ColGoal-ColBox-1);

                    sizeHachage=noeud_LEFT->hashCode();
                    // Si l’état n’est pas un doublon (table de Hachage)
                    if(myMap.find (sizeHachage)==myMap.end())
                    {
                        noeud_LEFT->setNparent(NoeudCourant);
                        //Ajouter A la table de Hachage
                        myMap.emplace(sizeHachage, noeud_LEFT);
                        //Lui ajouter une note #MasterChef
                        RankingNode.push_back(p_LEFT);
                        GoOnLeft=true;
                    }
                }

                if (canIPushABoxFromAccessibleZonePlayerInADirection(posBoxActu, ZAPparent, RIGHT, NoeudCourant->getNoeudPosBoxes()))
                {

                    //Nouvelle position de la box séléctionnée
                    posBoxes[i]=Coord::getDirPos(posBoxActu,RIGHT);

                    //on calcul la ZAP et la pos Normale
                    ZAPenfant=calculZAP(posBoxActu, posBoxes);
                    posNormMario=posNormalisee(ZAPenfant);

                    //On crée un noeud adapté
                    noeud_RIGHT=new Noeud(NULL, posBoxes, RIGHT, posNormMario,i);
                    Coord::coord2D(m_pos_boxes[i],lineBox,ColBox);
                    Coord::coord2D(m_pos_goals[i],LinGoal,ColGoal);
                    p_RIGHT=(LinGoal-lineBox)+(ColGoal-ColBox+1);

                    sizeHachage=noeud_RIGHT->hashCode();
                    // Si l’état n’est pas un doublon (table de Hachage)
                    if(myMap.find (sizeHachage)==myMap.end())
                    {
                        noeud_RIGHT->setNparent(NoeudCourant);
                        //Ajouter A la table de Hachage
                        myMap.emplace(sizeHachage, noeud_RIGHT);
                        //Lui ajouter une note #MasterChef
                        RankingNode.push_back(p_RIGHT);
                        GoOnRight=true;
                    }
                }

                std::sort(RankingNode.begin(),RankingNode.end());
                top_pushed=false;
                bot_pushed=false;
                left_pushed=false;
                right_pushed=false;


                for(unsigned short sizei = 3; sizei!=65535; sizei--){
                if (RankingNode.size()>sizei)
                {
                    if (RankingNode[sizei]==p_TOP&& !top_pushed&&GoOnTop)
                    {
                        _FileBestFS.push(noeud_TOP);
                        top_pushed=true;
                    }
                    else if (RankingNode[sizei]==p_BOTTOM&& !bot_pushed&&GoOnBottom)
                    {
                        _FileBestFS.push(noeud_BOTTOM);
                        bot_pushed=true;
                    }
                    else if (RankingNode[sizei]==p_LEFT&& !left_pushed&&GoOnLeft)
                    {
                        _FileBestFS.push(noeud_LEFT);
                        left_pushed=true;
                    }
                    else if (RankingNode[sizei]==p_RIGHT&& !right_pushed&&GoOnRight)
                    {
                        _FileBestFS.push(noeud_RIGHT);
                        right_pushed=true;
                    }

                }
                }
            }
        }
    }
    time=std::clock();
    int sec=(time-start)/1000;
    int cent=(time-start)/10;

    if(win)
    {
        std::cout << "You Win ! ... NOOOO ! The computer Win !" <<std::endl;
        visualisationWin(NoeudVict);

        //affichage du temps
        std::cout <<"Find in "<< sec <<","<< cent-sec*100 <<" secondes"<<std::endl;
        //affichage du nombre de noeuds traitées
        std::cout << cpt <<" node processed"<<std::endl;
    }
}

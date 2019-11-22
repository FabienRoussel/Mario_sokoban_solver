/************************************************************
Sokoban project - Maze file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/

#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include <functional>

#include "state.h"

#include <unordered_map>
#include <unordered_set>


// Max size for the field
#define NB_MAX_WIDTH     30
#define NB_MAX_HEIGHT    30


class Graphic;

enum
{
    SPRITE_GROUND = 0, SPRITE_WALL = 1, SPRITE_BOX = 2,
    SPRITE_BOX_PLACED = 3, SPRITE_GOAL = 4, SPRITE_MARIO = 5,
    SPRITE_DEADSQUARE = 9, SPRITE_ZAP =7
};

class Maze
{
    private:
        unsigned char m_lig, m_col;
        unsigned short m_pos_player;
        char m_dir_player;
        std::string m_level_path;
        //On crée un file de direction de Mario FIFO
        std::queue<char> m_dirMario;

        std::vector<unsigned char> m_field; // field
        std::vector<unsigned short> m_pos_boxes; // box positions
        std::vector<unsigned short> m_pos_goals; // goal positions
        bool m_actifDeadlock;

        friend std::ostream& operator << (std::ostream& O, const Maze& n);

        bool _load(const std::string& path);
        bool _isCompleted() const;
        bool _canPushBox(unsigned short posBox, char dir, unsigned short& newPosBox) const;

        void solvIDA(bool isBestFirst);

    public:
        void setActDL();

        //is completed version public
        bool _isFinish() const{return this->_isCompleted();};
        Maze(const std::string& path);
        ~Maze();

        bool init();
        bool updatePlayer(char dir);
        void draw(const Graphic& g) const;

        // Specific getters for field
        bool isSquareWalkable(unsigned short pos) const;
        bool isSquareGround(unsigned short pos) const;
        bool isSquareBox(unsigned short pos) const;
        bool isSquareDead(unsigned short pos) const;
        bool isSquareGoal(unsigned short pos) const;
        bool isSquareWall(unsigned short pos) const;
        bool isSquareBoxPlaced(unsigned short pos) const;

        // Other getters
        const std::string& getLevelPath() const;
        unsigned short getPosPlayer() const;
        unsigned int getSize() const;
        unsigned int getCol() const;
        void setSquare(unsigned short pos, unsigned char s);
        const std::vector<unsigned char>& getField() const;
        const std::vector<unsigned short>& getGoals() const;
        std::vector<unsigned short> getPosBoxes() const;
        const std::vector<unsigned short>& getRefPosBoxes() const;
        bool isSquareDeadlock(short unsigned posBox, short unsigned dir);
        bool isSquareDeadlock(short unsigned posBox, short unsigned dir, const std::vector<unsigned short>& posBoxes);
        std::queue<char>& getdirMario();

        // Setter
        void setPlayerPos(unsigned short p);
        void setPosBox(unsigned short i, unsigned short pos);
        void setdirMario(std::queue<char> dirMario){m_dirMario=dirMario;};

        //Other programs
        void swapTwoSquare(unsigned short pos1, unsigned short pos2);
        void deadlocksDetectionActu();
        void sleep(unsigned int mseconds);
        void deadlocksStatic();
        void GroundLine( unsigned short position, bool& ground);

        //Algotithm od artifical intelligence
        std::unordered_set<short unsigned> calculZAP(short unsigned posPlayer, const std::vector<unsigned short>& PosBoxes);
        unsigned short posNormalisee(std::unordered_set<short unsigned>& ZAP);
        bool canIPushABoxFromAccessibleZonePlayerInADirection(unsigned short int boxPos, std::unordered_set<unsigned short>& ZAP, char dir, const std::vector<unsigned short>& pos_boxes);

        std::vector<char> Bruteforce(bool win,std::vector<char>& direction, Graphic g);
        void bruteForce();
        void solvBFS();
        void solvDFS();
        void solvBestFS();
        void solvIDA();
        unsigned int heuristic(Noeud* noeud);

        ///void findPath(Etape* winStep);
        void displayPath(Graphic g, unsigned short msec);
        void visualisationWin(Noeud *lastNoeud);
        unsigned short BFSplayer(Noeud *pred, Noeud *succ, unsigned short posMario);

        void pBestFS();
};


// Ceci est la fonction comparatrice utilisée pour ordonner les noeuds, lors de la résolution par IDA*
class heuristicCompare{
public:

    bool operator() (Noeud* node1, Noeud* node2)
    {
        if(node1->isAStar())
        {
            if( (node1->getCost() + node1->getHCost()) < (node2->getCost() + node2->getHCost()) ) return true;
            else return false;
        }
        else if( node1->getHCost() < node2->getHCost()) return true;
        else return false;
    }
};


// Inline implementation of getters and setters (DO NOT TOUCH !)

inline void Maze::setPlayerPos(unsigned short p) {
    if (p < this->getSize()) // NB: p always > 0 because unsigned short
        this->m_pos_player = p;
}

//Mettre a jour une case des box
inline void Maze::setPosBox(unsigned short i, unsigned short pos) {
    m_pos_boxes[i]=pos;
}

inline const std::string& Maze::getLevelPath() const { return this->m_level_path; }
inline const std::vector<unsigned short>& Maze::getGoals() const { return this->m_pos_goals; }
inline const std::vector<unsigned char>& Maze::getField() const { return this->m_field; }

inline unsigned int Maze::getSize() const { return this->m_field.size(); }
inline unsigned int Maze::getCol() const { return this->m_col; }
inline std::vector<unsigned short> Maze::getPosBoxes() const { return m_pos_boxes; }
inline const std::vector<unsigned short>& Maze::getRefPosBoxes() const { return m_pos_boxes; } ///
inline void Maze::setActDL() { m_actifDeadlock=!m_actifDeadlock; std::cout << m_actifDeadlock << std::endl;}
inline unsigned short Maze::getPosPlayer() const { return m_pos_player;}
inline std::queue<char>& Maze::getdirMario(){ return m_dirMario;}


inline void Maze::setSquare(unsigned short pos, unsigned char s)
{
    if (pos < (this->m_lig*this->m_col))
    {
        this->m_field[pos] = s;
    }
}

inline bool Maze::isSquareWalkable(unsigned short pos) const
{
    return ((this->m_field[pos] == SPRITE_GROUND || this->m_field[pos] == SPRITE_GOAL) ? true : false);
}

inline bool Maze::isSquareDead(unsigned short pos) const
{
    return ((this->m_field[pos] == SPRITE_DEADSQUARE) ? true : false);
}

inline bool Maze::isSquareGround(unsigned short pos) const
{
    return ((this->m_field[pos] == SPRITE_GROUND) ? true : false);
}

inline bool Maze::isSquareBox(unsigned short pos) const
{
    return ((this->m_field[pos] == SPRITE_BOX || this->m_field[pos] == SPRITE_BOX_PLACED) ? true : false);
}

inline bool Maze::isSquareGoal(unsigned short pos) const
{
    return (this->m_field[pos] == SPRITE_GOAL ? true : false);
}

inline bool Maze::isSquareWall(unsigned short pos) const
{
    return (this->m_field[pos] == SPRITE_WALL ? true : false);
}

inline bool Maze::isSquareBoxPlaced(unsigned short pos) const
{
    return (this->m_field[pos] == SPRITE_BOX_PLACED ? true : false);
}

#endif // MAZE_H_INCLUDED
/**

void Maze::pBestFS()
{
    //Pour mesurer le temps
    clock_t start;
    clock_t time;
    start = std::clock();


    bool win=false;
    const char directions[4] = {TOP, BOTTOM, LEFT, RIGHT};

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
    std::vector<Noeud*> noeud_T_B_L_R;
    noeud_T_B_L_R.resize(4);

    //On le push et marque
    marquage.push_back(NoeudCourant);
    _FileBestFS.push(NoeudCourant);
    int cpt=0;

    //Pour le BestFS, regarder le poids des direction
    unsigned short p_TOP=0;
    unsigned short p_BOTTOM=0;
    unsigned short p_LEFT=0;
    unsigned short p_RIGHT=0;
    std::vector <unsigned short> poids_T_B_L_R;
    poids_T_B_L_R.resize(4);

    unsigned int lineBox,ColBox,LinGoal,ColGoal;
    bool top_pushed=false;
    bool bot_pushed=false;
    bool left_pushed=false;
    bool right_pushed=false;
    bool GoOnTop=false;
    bool GoOnBottom=false;
    bool GoOnRight=false;
    bool GoOnLeft=false;
    std::vector<bool> goON_T_B_L_R;
    for(unsigned short i=0; i<4; i++) goON_T_B_L_R.push_back(false);

    unsigned short j;

    // Debut BestfS
    while(!_FileBestFS.empty() && !win)
    {
        //Compteur de noeud traité
        cpt++;

        NoeudCourant=_FileBestFS.front();
        _FileBestFS.pop();
std::cout << " PB:" << NoeudCourant->getNoeudPosBoxes()[1] << " PM:"
<< NoeudCourant->getPosNormMario() << " DIR"<< NoeudCourant->getDirBoxPush() << " IND" << NoeudCourant->getIndexBosPush() << std::endl;
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

                //On sauvegarde la position des caisses
                posBoxes=NoeudCourant->getNoeudPosBoxes();
                std::vector<int>RankingNode;
                j=0;
                for (char dir: directions)
                {
                    goON_T_B_L_R[j]=false;
                    if (canIPushABoxFromAccessibleZonePlayerInADirection(posBoxActu, ZAPparent, dir, NoeudCourant->getNoeudPosBoxes()))
                    {
                        //Nouvelle position de la box séléctionné
                        posBoxes[i]=Coord::getDirPos(posBoxActu,dir);

                        //on calcul la ZAP et la pos Normale
                        ZAPenfant=calculZAP(posBoxActu, posBoxes);
                        posNormMario=posNormalisee(ZAPenfant);

                        //On crée un noeud adapté
                        noeud_T_B_L_R[j]=new Noeud(NULL, posBoxes, dir, posNormMario,i);
                        Coord::coord2D(m_pos_boxes[i],lineBox,ColBox);
                        Coord::coord2D(m_pos_goals[i],LinGoal,ColGoal);
                        if (j==0) poids_T_B_L_R[j]=(LinGoal-lineBox-1)+(ColGoal-ColBox);
                        else if (j==1) poids_T_B_L_R[j]=(LinGoal-lineBox+1)+(ColGoal-ColBox);
                        else if (j==2) poids_T_B_L_R[j]=(LinGoal-lineBox)+(ColGoal-ColBox-1);
                        else poids_T_B_L_R[j]=(LinGoal-lineBox)+(ColGoal-ColBox+1);

                        sizeHachage=noeud_T_B_L_R[j]->hashCode();
                        // Si l’état n’est pas un doublon (table de Hachage)
                        if(myMap.find (sizeHachage)==myMap.end())
                        {
                            std::cout << "PM" << posNormMario << "  " ;
                            std::cout  << (int)dir << " "  << i<< std::endl<< std::endl;
                            noeud_T_B_L_R[j]->setNparent(NoeudCourant);
                            //Ajouter A la table de Hachage
                            myMap.emplace(sizeHachage, noeud_T_B_L_R[j]);
                            //Lui ajouter une note #MasterChef
                            RankingNode.push_back(poids_T_B_L_R[j]);
                            goON_T_B_L_R[j]=true;
                        }
                        else std::cout << "prob" << (int)dir << " " ;
                    }
                    j++;

                }

                std::sort(RankingNode.begin(),RankingNode.end());
                top_pushed=false;
                bot_pushed=false;
                left_pushed=false;
                right_pushed=false;

                for(unsigned short sizei = 3; sizei!=65535; sizei--){
                if (RankingNode.size()>sizei)
                {
                    j=0;
                    if (RankingNode[sizei]==p_TOP&& !top_pushed&&goON_T_B_L_R[0])
                    {
                        _FileBestFS.push(noeud_T_B_L_R[0]);
                        top_pushed=true;
                    }
                    else if (RankingNode[sizei]==p_BOTTOM&& !bot_pushed&&goON_T_B_L_R[1])
                    {
                        _FileBestFS.push(noeud_T_B_L_R[1]);
                        bot_pushed=true;
                    }
                    else if (RankingNode[sizei]==p_LEFT&& !left_pushed&&goON_T_B_L_R[2])
                    {
                        _FileBestFS.push(noeud_T_B_L_R[2]);
                        left_pushed=true;
                    }
                    else if (RankingNode[sizei]==p_RIGHT&& !right_pushed&&goON_T_B_L_R[3])
                    {
                        _FileBestFS.push(noeud_T_B_L_R[3]);
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
std::cout << cpt <<" node tested"<<std::endl;

    if(win)
    {
        std::cout << "You Win ! ... NOOOO ! The computer Win !" <<std::endl;
        visualisationWin(NoeudVict);

        //affichage du tmps
        std::cout <<"Find in "<< sec <<","<< cent-sec*100 <<" secondes"<<std::endl;
        //affichage du compteur
        std::cout << cpt <<" node tested"<<std::endl;

    }

}
//*/

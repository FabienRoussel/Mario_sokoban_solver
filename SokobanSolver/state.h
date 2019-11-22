#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <iostream>
#include <vector>
#include <sstream>

class Noeud
{
private:
    //Pointeur sur le pr�decesseur
    Noeud* m_parent ;
    //Vecteur qui contient les positions des boxes � un moment donn�
	std::vector<unsigned short> m_pos_boxes ;
	//Contient la direction de la caisse pouss�e
	unsigned short m_dir_caisse_push ;
	//Permet d'�viter les doublons
	unsigned short m_posNormal_mario ; ///position en haut � gauche accessible de Mario
	//On a besoin de savoir quelle caisse est pouss�e
	unsigned short m_index_caisse_push ; ///li� au std vector plus haut
	// Cout Heuristique (nombre de mouvements estim� pour terminer le niveau)
	unsigned int m_heuristicCost ;
	// permet de changer un BestFS en A* plus facilement (modifie l'op�rateur " < " du Noeud en fonction de l'algorithme � calculer)
	bool m_isAStar;

    // fonction priv�e r�cursive pour obtenir le cout dans l'arbre
    unsigned int getCost(int cost) const;

public:
    //Constructor
    Noeud(Noeud* parent, std::vector<unsigned short> pos_boxes,	unsigned short dir_caisse_push,
          unsigned short posNormal_mario, unsigned short index_caisse_push );

    Noeud(Noeud* parent, std::vector<unsigned short> pos_boxes,	unsigned short dir_caisse_push,
          unsigned short posNormal_mario, unsigned short index_caisse_push, unsigned int heuristicCost, bool isAStar );
    //Destructor
    ~Noeud();

    // Operator overload
    bool operator<(Noeud node2);
    //Getters
    Noeud* getNParent() const;
    std::vector<unsigned short> getNoeudPosBoxes() const;
    const std::vector<unsigned short>& getNoeudRefPosBoxes() const;
    unsigned short getDirBoxPush() const;
    unsigned short getPosNormMario() const;
    unsigned short getIndexBosPush() const;
    unsigned int   getCost() const;
    unsigned int   getHCost() const;
    bool isAStar() const;

    //Setters
    void setNparent(Noeud* parent) ;
    void setNoeudPosBoxes(std::vector<unsigned short> posBox);
    void setDirBoxPush(unsigned short DirBoxPush);
    void setPosNormMario(unsigned short PosNorm);
    void setIndexBosPush(unsigned short IndexBox);
    void setHCost(unsigned int heuristicCost);
    //fonction de hashage
    size_t hashCode();
};

/*
typedef struct Noeud {
	Noeud* m_parent ;
	std::vector<unsigned short> m_pos_boxes ;
	unsigned short dir_caisse_push ;
	unsigned short m_position_normalisee_de_mario ; ///position en haut � gauche accessible de Mario
	unsigned short m_index_caisse_push ; ///li� au std vector plus haut
}n_Noeud ;
*/

#endif // STATE_H_INCLUDED

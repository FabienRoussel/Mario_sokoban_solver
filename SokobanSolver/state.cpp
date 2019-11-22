#include "state.h"

//Constructor
Noeud::Noeud(Noeud* parent, std::vector<unsigned short> pos_boxes,	unsigned short dir_caisse_push,
      unsigned short posNormal_mario, unsigned short index_caisse_push )
{
    m_parent = parent;
    m_pos_boxes = pos_boxes;
    m_dir_caisse_push = dir_caisse_push;
    m_posNormal_mario = posNormal_mario;
    m_index_caisse_push = index_caisse_push;

}

Noeud::Noeud(Noeud* parent, std::vector<unsigned short> pos_boxes,	unsigned short dir_caisse_push,
      unsigned short posNormal_mario, unsigned short index_caisse_push, unsigned int heuristicCost, bool isAStar)
{
    m_parent = parent;
    m_pos_boxes = pos_boxes;
    m_dir_caisse_push = dir_caisse_push;
    m_posNormal_mario = posNormal_mario;
    m_index_caisse_push = index_caisse_push;
    m_heuristicCost = heuristicCost;
    m_isAStar = isAStar;
}
//Destructor
Noeud::~Noeud()
{
    m_pos_boxes.clear();
}

//Getters
Noeud* Noeud::getNParent() const
{
    return m_parent;
}
std::vector<unsigned short> Noeud::getNoeudPosBoxes() const
{
    return m_pos_boxes;
}
const std::vector<unsigned short>& Noeud::getNoeudRefPosBoxes() const
{
    return m_pos_boxes;
}
unsigned short Noeud::getDirBoxPush() const
{
    return m_dir_caisse_push;
}
unsigned short Noeud::getPosNormMario() const
{
    return m_posNormal_mario;
}
unsigned short Noeud::getIndexBosPush() const
{
    return m_index_caisse_push;
}
unsigned int Noeud::getCost() const
{
    return getCost(0);
}

unsigned int Noeud::getCost(int cost) const
{
    if(getNParent() != NULL) return getNParent()->getCost(++cost);
    else return cost;
}

unsigned int Noeud::getHCost() const
{
    return m_heuristicCost;
}

bool Noeud::isAStar() const
{
    return m_isAStar;
}

//Setters
void Noeud::setNparent(Noeud* parent)
{
    m_parent=parent;
}
void Noeud::setNoeudPosBoxes(std::vector<unsigned short> posBox)
{
    m_pos_boxes=posBox;
}
void Noeud::setDirBoxPush(unsigned short DirBoxPush)
{
    m_dir_caisse_push=DirBoxPush;
}
void Noeud::setPosNormMario(unsigned short PosNorm)
{
    m_posNormal_mario=PosNorm;
}
void Noeud::setIndexBosPush(unsigned short IndexBox)
{
    m_index_caisse_push=IndexBox;
}
void Noeud::setHCost(unsigned int heuristicCost)
{
    m_heuristicCost = heuristicCost;
}

size_t Noeud::hashCode()
{
    std::hash<std::string> hash_fn;
    std::stringstream myString;
    myString << m_posNormal_mario;

    for(auto elem: m_pos_boxes)
    {
        myString << ","<< elem;
    }
    return (hash_fn(myString.str()));
}

// Ceci est la fonction comparatrice utilisée pour ordonner les noeuds, lors de la résolution par IDA*
bool Noeud::operator<(Noeud node2)
{
    if(m_isAStar) return (getCost() + getHCost()) > (node2.getCost() + node2.getHCost());
    else return getHCost() > node2.getHCost();
}

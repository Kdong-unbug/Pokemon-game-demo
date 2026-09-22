#include "item.h"

Item::Item(int id, QString name):tool_id(id),tool_name(name)
{

}

int Item::getID() const{ return tool_id; }
QString Item::getName() const{ return tool_name; }

// Potion
Potion::Potion(int id, QString name, int healValue)
    :Item(id,name),m_healValue(healValue)
{}

void Potion::use(){}
int Potion::getHeal() const{ return m_healValue; }

// Pokeball
Pokeball::Pokeball(int id, QString name, float rateBonus)
    :Item(id,name),m_rateBonus(rateBonus)
{}

void Pokeball::use(){}
float Pokeball::getRateBonus() const{ return m_rateBonus; }

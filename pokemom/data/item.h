#ifndef ITEM_H
#define ITEM_H
#include <QString>

#define ITEM_POTION   1
#define ITEM_POKEBALL 2

class Item
{
public:
    Item(int id, QString name);
    virtual ~Item() = default;
    int getID() const;
    QString getName() const;
    virtual void use() = 0;
protected:
    int tool_id;
    QString tool_name;
    Item();
};

class Potion : public Item
{
public:
    Potion(int id, QString name, int healValue);
    void use() override;
    int getHeal() const;
private:
    int m_healValue;
};

class Pokeball : public Item
{
public:
    Pokeball(int id, QString name, float rateBonus);
    void use() override;
    float getRateBonus() const;
private:
    float m_rateBonus;
};

#endif

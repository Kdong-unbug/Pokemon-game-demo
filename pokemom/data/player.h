#ifndef PLAYER_H
#define PLAYER_H

#include "pokemon.h"
#include "bag.h"
#include <QVector>
#include <QString>
#include <QPoint>
#include <QJsonObject>

class Player
{
public:
    Player();

    QString playerName;
    int money;
    QVector<Pokemon> team;
    int currentPokemonIndex;
    Bag bag;
    QVector<Pokemon> storage;

    void addPokemon(const Pokemon& pokemon);
    Pokemon& getCurrentPokemon();
    void setCurrentPokemon(int index);
    bool isTeamFull() const;
    int getTeamSize() const;
    bool hasHealthyPokemon() const;
    void healAll();

    void addToStorage(const Pokemon& pokemon);
    bool isStorageFull() const;
    int getStorageSize() const;

    void gainMoney(int amount);
    bool spendMoney(int amount);

    // 序列化
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);
};

#endif

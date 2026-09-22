#ifndef POKEMON_H
#define POKEMON_H
#include "skill.h"
#include <QString>
#include <QVector>
#include <QJsonObject>
#include "gamedefine.h"


class Pokemon
{
public:
        QString Pokemonname;//精灵名
        int level;//等级
        int exp;//当前经验值
        int maxexp;//升级所需经验值
        int hp;//当前血量;
        int maxhp;//最大血量;
        int attack;//攻击力;
        int defense;//防御力;
        int speed;//速度
        int elemType;//属性
        StatusEffect status;//战斗异常状态
        int statusTurns;//剩余睡眠回合
        QString resFrontImg;   // 正面精灵图（敌方）0
        QString resBackImg;    //背面图(我方)1

         QJsonObject toJson() const;
         void fromJson(const QJsonObject &obj);
public:
    void gainExp(int amount);
    bool checkLevelUp(); // 返回是否升级


        QVector<Skill> skills;

    Pokemon();   //声明
};

#endif // POKEMON_H

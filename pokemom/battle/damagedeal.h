#ifndef DAMAGEDEAL_H
#define DAMAGEDEAL_H

#include "pokemon.h"
#include "skill.h"
#include "gamedefine.h"


struct DamageResult
{
    bool isHit;//是否打中
    int damage;
    double rate;
    bool isCrit;
    StatusEffect appliedEffect;
};

class damagedeal
{
public:
        damagedeal();
    //单纯伤害计算
    static DamageResult calculate(const Skill& skill, const Pokemon& attacker, const Pokemon& defender);

    //获取属性
    static double getTypeRate(int skillType, int defType);
    static QString getStatusName(int effect);

};

#endif // DAMAGEDEAL_H

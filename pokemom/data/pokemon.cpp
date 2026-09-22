#include "pokemon.h"
#include "allpokemon.h"
#include <QJsonArray>

Pokemon::Pokemon()
{
    Pokemonname = "";
    level = 1;
    exp = 0;
    maxexp = 50;
    hp = 20;
    maxhp = 20;
    attack = 8;
    defense = 8;
    speed = 8;
    elemType = static_cast<int>(ElementType::Normal);
    status = StatusEffect::None;
    statusTurns = 0;
    resFrontImg = "";
    resBackImg = "";
}

void Pokemon::gainExp(int amount)
{
    exp += qMax(0, amount);
    while (maxexp > 0 && exp >= maxexp) {
        exp -= maxexp;
        level++;
        maxexp = level * 12 + 20;      // 升级所需经验递增
        maxhp += 5 + level * 2;
        hp = maxhp;                    // 升级时回满（本项目没有精灵中心，先保持可玩）
        attack += 1 + level / 2;
        defense += 1 + level / 2;
        speed += 1 + level / 4;
    }
    if (level > 100)
        level = 100;
}

bool Pokemon::checkLevelUp()
{
    return maxexp > 0 && exp >= maxexp;
}

QJsonObject Pokemon::toJson() const
{
    QJsonObject obj;
    obj["name"] = Pokemonname;
    obj["level"] = level;
    obj["exp"] = exp;
    obj["maxexp"] = maxexp;
    obj["hp"] = hp;
    obj["maxhp"] = maxhp;
    obj["attack"] = attack;
    obj["defense"] = defense;
    obj["speed"] = speed;
    obj["elemType"] = elemType;
    obj["status"] = static_cast<int>(status);
    obj["statusTurns"] = statusTurns;
    obj["frontImg"] = resFrontImg;
    obj["backImg"] = resBackImg;

    QJsonArray skillArray;
    for (const Skill& s : skills) {
        QJsonObject skillObj;
        skillObj["name"] = s.Skillname;
        skillObj["power"] = s.power;
        skillObj["type"] = s.skilltype;
        skillObj["accuracy"] = s.accuracy;
        skillObj["critRate"] = s.critRate;
        skillObj["effect"] = s.effect;
        skillObj["effectChance"] = s.effectChance;
        skillObj["priority"] = s.priority;
        skillArray.append(skillObj);
    }
    obj["skills"] = skillArray;
    return obj;
}

void Pokemon::fromJson(const QJsonObject& obj)
{
    Pokemonname = obj["name"].toString();
    level = qMax(1, obj["level"].toInt(1));
    exp = qMax(0, obj["exp"].toInt(0));
    maxexp = qMax(1, obj["maxexp"].toInt(50));
    hp = obj["hp"].toInt(20);
    maxhp = qMax(1, obj["maxhp"].toInt(20));
    attack = obj["attack"].toInt(8);
    defense = obj["defense"].toInt(8);
    speed = obj["speed"].toInt(8);
    elemType = qBound(0, obj["elemType"].toInt(0), 3);
    status = static_cast<StatusEffect>(qBound(0, obj["status"].toInt(0), 3));
    statusTurns = qMax(0, obj["statusTurns"].toInt(0));
    resFrontImg = obj["frontImg"].toString();
    resBackImg = obj["backImg"].toString();

    hp = qBound(0, hp, maxhp);

    skills.clear();
    QJsonArray skillArray = obj["skills"].toArray();
    for (const QJsonValue& val : skillArray) {
        QJsonObject sObj = val.toObject();
        Skill s;
        s.Skillname = sObj["name"].toString();
        s.power = qMax(0, sObj["power"].toInt(0));
        s.skilltype = qBound(0, sObj["type"].toInt(0), 3);
        s.accuracy = qBound(1, sObj["accuracy"].toInt(100), 100);
        s.critRate = qBound(0.0, sObj["critRate"].toDouble(0.0), 1.0);
        s.effect = qBound(0, sObj["effect"].toInt(0), 3);
        s.effectChance = qBound(0, sObj["effectChance"].toInt(0), 100);
        s.priority = sObj["priority"].toInt(0);
        skills.append(s);
    }

    // 旧存档可能少于4个技能，读档后按种族补齐
    allpokemon::ensureSpeciesSkills(*this);
}
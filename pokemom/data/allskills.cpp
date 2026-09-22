#include "allskills.h"
#include "gamedefine.h"

QList<Skill> AllSkills::skillLib;

void AllSkills::init()
{
    skillLib.clear();

    auto makeSkill = [](const QString &name, int type, int power, int accuracy,
                        double critRate, int effect, int effectChance, int priority) {
        Skill skill;
        skill.Skillname = name;
        skill.skilltype = type;
        skill.power = power;
        skill.accuracy = accuracy;
        skill.critRate = critRate;
        skill.effect = effect;
        skill.effectChance = effectChance;
        skill.priority = priority;
        return skill;
    };

    skillLib.append(makeSkill("火花",     static_cast<int>(ElementType::Fire),  45, 100, 0.06, static_cast<int>(StatusEffect::Burn),  15, 0));
    skillLib.append(makeSkill("缠绕",     static_cast<int>(ElementType::Grass), 38,  95, 0.06, static_cast<int>(StatusEffect::None),   0, 0));
    skillLib.append(makeSkill("水枪",     static_cast<int>(ElementType::Water), 45, 100, 0.06, static_cast<int>(StatusEffect::None),   0, 0));
    skillLib.append(makeSkill("撞击",     static_cast<int>(ElementType::Normal),35, 100, 0.06, static_cast<int>(StatusEffect::None),   0, 0));
    skillLib.append(makeSkill("电光一闪", static_cast<int>(ElementType::Normal),30, 100, 0.06, static_cast<int>(StatusEffect::None),   0, 1));
    skillLib.append(makeSkill("火焰牙",   static_cast<int>(ElementType::Fire),  60,  95, 0.08, static_cast<int>(StatusEffect::Burn),  15, 0));
    skillLib.append(makeSkill("水流喷射", static_cast<int>(ElementType::Water), 40, 100, 0.06, static_cast<int>(StatusEffect::None),   0, 1));
    skillLib.append(makeSkill("藤鞭",     static_cast<int>(ElementType::Grass), 45, 100, 0.06, static_cast<int>(StatusEffect::Poison), 10, 0));
    skillLib.append(makeSkill("催眠粉",   static_cast<int>(ElementType::Grass),  0,  75, 0.00, static_cast<int>(StatusEffect::Sleep), 70, 0));
    skillLib.append(makeSkill("泡沫光线", static_cast<int>(ElementType::Water), 55,  95, 0.06, static_cast<int>(StatusEffect::None),   0, 0));
    skillLib.append(makeSkill("飞叶快刀", static_cast<int>(ElementType::Grass), 55,  95, 0.12, static_cast<int>(StatusEffect::None),   0, 0));
    skillLib.append(makeSkill("火焰轮",   static_cast<int>(ElementType::Fire),  55,  95, 0.06, static_cast<int>(StatusEffect::Burn),  10, 0));
}

Skill AllSkills::getSkill(SkillId id)
{
    int idx = static_cast<int>(id);
    if (idx < 0 || idx >= skillLib.size())
        return Skill();
    return skillLib.at(idx);
}

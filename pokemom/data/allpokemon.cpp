#include "allpokemon.h"
#include "pokemon.h"
#include "skill.h"
#include "allskills.h"
#include <QtGlobal>

// 静态容器内存定义
QVector<Pokemon> allpokemon::m_templateList;

void allpokemon::initAllTemplate()
{
    m_templateList.clear();

    Pokemon firefox;
    firefox.Pokemonname = "小火狐";
    firefox.elemType = static_cast<int>(ElementType::Fire);
    firefox.level = 5;
    firefox.exp = 0;
    firefox.maxexp = 80;
    firefox.maxhp = 38;
    firefox.hp = firefox.maxhp;
    firefox.attack = 15;
    firefox.defense = 10;
    firefox.speed = 14;
    firefox.resFrontImg = ":/new/prefix1/pic/firefox0.png";
    firefox.resBackImg  = ":/new/prefix1/pic/firefox1.png";
    ensureSpeciesSkills(firefox);
    m_templateList.append(firefox);

    Pokemon otter;
    otter.Pokemonname = "小水獭";
    otter.elemType = static_cast<int>(ElementType::Water);
    otter.level = 5;
    otter.exp = 0;
    otter.maxexp = 80;
    otter.maxhp = 42;
    otter.hp = otter.maxhp;
    otter.attack = 13;
    otter.defense = 12;
    otter.speed = 11;
    otter.resFrontImg = ":/new/prefix1/pic/otter0.png";
    otter.resBackImg  = ":/new/prefix1/pic/otter0.png";
    ensureSpeciesSkills(otter);
    m_templateList.append(otter);

    Pokemon deer;
    deer.Pokemonname = "小菇鹿";
    deer.elemType = static_cast<int>(ElementType::Grass);
    deer.level = 5;
    deer.exp = 0;
    deer.maxexp = 80;
    deer.maxhp = 40;
    deer.hp = deer.maxhp;
    deer.attack = 12;
    deer.defense = 14;
    deer.speed = 10;
    deer.resFrontImg = ":/new/prefix1/pic/deer0.png";
    deer.resBackImg  = ":/new/prefix1/pic/deer1.png";
    ensureSpeciesSkills(deer);
    m_templateList.append(deer);
}

Pokemon allpokemon::getTemplate(int index)
{
    if (index >= 0 && index < m_templateList.size())
        return m_templateList[index];
    return Pokemon();
}

namespace {
bool hasSkillName(const Pokemon& pokemon, const QString& name)
{
    for (const Skill& s : pokemon.skills) {
        if (s.Skillname == name)
            return true;
    }
    return false;
}

void appendSkillOnce(Pokemon& pokemon, SkillId id, int maxSkills)
{
    Skill skill = AllSkills::getSkill(id);
    if (skill.Skillname.isEmpty() ||
        pokemon.skills.size() >= maxSkills ||
        hasSkillName(pokemon, skill.Skillname)) {
        return;
    }
    pokemon.skills.append(skill);
}
}

void allpokemon::ensureSpeciesSkills(Pokemon& pokemon)
{
    const int maxSkills = 4;

    if (pokemon.Pokemonname == "小火狐") {
        const SkillId moves[] = {SkillId::Spark, SkillId::QuickAttack,
                                 SkillId::FireFang, SkillId::Tackle};
        for (SkillId id : moves)
            appendSkillOnce(pokemon, id, maxSkills);
    }
    else if (pokemon.Pokemonname == "小水獭") {
        const SkillId moves[] = {SkillId::WaterGun, SkillId::AquaJet,
                                 SkillId::BubbleBeam, SkillId::Tackle};
        for (SkillId id : moves)
            appendSkillOnce(pokemon, id, maxSkills);
    }
    else if (pokemon.Pokemonname == "小菇鹿") {
        const SkillId moves[] = {SkillId::VineWhip, SkillId::RazorLeaf,
                                 SkillId::Wrap, SkillId::SleepPowder};
        for (SkillId id : moves)
            appendSkillOnce(pokemon, id, maxSkills);
    }

    while (pokemon.skills.size() > maxSkills)
        pokemon.skills.removeLast();
}

Pokemon allpokemon::createInstance(int index, int level)
{
    Pokemon pokemon = getTemplate(index);
    if (pokemon.Pokemonname.isEmpty())
        return pokemon;

    pokemon.level = qBound(1, level, 100);
    pokemon.maxhp = qMax(12, pokemon.maxhp + (pokemon.level - 5) * 4);
    pokemon.attack = qMax(3, pokemon.attack + (pokemon.level - 5) * 2);
    pokemon.defense = qMax(3, pokemon.defense + (pokemon.level - 5) * 2);
    pokemon.speed = qMax(3, pokemon.speed + (pokemon.level - 5));
    pokemon.hp = pokemon.maxhp;
    pokemon.exp = 0;
    pokemon.maxexp = pokemon.level * 12 + 20;
    pokemon.status = StatusEffect::None;
    pokemon.statusTurns = 0;

    ensureSpeciesSkills(pokemon);
    return pokemon;
}
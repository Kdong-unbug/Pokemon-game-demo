#include "wildencounter.h"
#include "allpokemon.h"
#include <QRandomGenerator>
#include <QtGlobal>

WildEncounterManager::WildEncounterManager()
{
    // 地图1左侧：靠近小镇的低等级草丛
    Rule routeLow;
    routeLow.mapIndex = 1;
    routeLow.chancePercent = 6;
    routeLow.minLevel = 3;
    routeLow.maxLevel = 6;
    routeLow.zones.append(QRect(80, 80, 520, 840));
    routeLow.species = { {0, 2}, {1, 2}, {2, 1} };
    m_rules.append(routeLow);

    // 地图1右侧：深处的较高等级区域
    Rule routeHigh;
    routeHigh.mapIndex = 1;
    routeHigh.chancePercent = 5;
    routeHigh.minLevel = 6;
    routeHigh.maxLevel = 9;
    routeHigh.zones.append(QRect(660, 80, 560, 840));
    routeHigh.species = { {0, 1}, {1, 1}, {2, 2} };
    m_rules.append(routeHigh);
}

bool WildEncounterManager::roll(const QPoint& playerCenter, int mapIndex, Pokemon& result)
{
    bool hasZoneOnMap = false;
    for (const Rule& rule : m_rules) {
        if (rule.mapIndex != mapIndex)
            continue;

        // 玩家不在该规则的可遇敌区域内时，尝试下一条规则
        if (!isInsideZones(rule.zones, playerCenter))
            continue;
        hasZoneOnMap = true;

        // 本次移动没有触发，继续尝试同地图的其他区域
        if (QRandomGenerator::global()->bounded(100) >= rule.chancePercent)
            continue;

        int totalWeight = 0;
        for (const Species& sp : rule.species)
            totalWeight += sp.weight;
        if (totalWeight <= 0)
            continue;

        int pick = QRandomGenerator::global()->bounded(totalWeight);
        int speciesIndex = 0;
        for (int i = 0; i < rule.species.size(); ++i) {
            pick -= rule.species.at(i).weight;
            if (pick < 0) {
                speciesIndex = rule.species.at(i).templateIndex;
                break;
            }
        }

        int level = rule.minLevel + QRandomGenerator::global()->bounded(rule.maxLevel - rule.minLevel + 1);
        result = allpokemon::createInstance(speciesIndex, level);
        if (!result.Pokemonname.isEmpty())
            return true;
    }

    Q_UNUSED(hasZoneOnMap);
    return false;
}

bool WildEncounterManager::isInsideZones(const QVector<QRect>& zones, const QPoint& pos) const
{
    if (zones.isEmpty())
        return true;
    for (const QRect& zone : zones) {
        if (zone.contains(pos))
            return true;
    }
    return false;
}
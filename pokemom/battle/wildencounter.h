#ifndef WILDENCOUNTER_H
#define WILDENCOUNTER_H

#include "pokemon.h"
#include <QPoint>
#include <QRect>
#include <QVector>

// 按地图/区域配置野生精灵，负责“是否能遇敌”和“遇到什么”的随机规则
class WildEncounterManager
{
public:
    WildEncounterManager();

    // 玩家中心点在某地图的可遇敌区域时，按概率生成一只野生宝可梦
    // 返回 true 时 result 会被填充为有效精灵
    bool roll(const QPoint& playerCenter, int mapIndex, Pokemon& result);

private:
    struct Species {
        int templateIndex;
        int weight;
    };
    struct Rule {
        int mapIndex;
        int chancePercent;      // 每次移动判定的遇敌率 0~100
        int minLevel;
        int maxLevel;
        QVector<QRect> zones;   // 可遇敌区域；为空表示全图可遇敌
        QVector<Species> species;
    };

    QVector<Rule> m_rules;
    bool isInsideZones(const QVector<QRect>& zones, const QPoint& pos) const;
};

#endif // WILDENCOUNTER_H
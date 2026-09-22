#ifndef BATTLESTATE_H
#define BATTLESTATE_H

#include "pokemon.h"
#include "skill.h"
#include "gamedefine.h"
#include "damagedeal.h"
#include <QString>

class battlestate
{
public:
    battlestate();

    // 战斗状态与双方精灵副本
    BattleState state;
    Pokemon playerPoke;
    Pokemon enemyPoke;
    // 最近一次行动的完整描述，供 UI 直接显示
    QString lastLog;

    void setupBattle(const Pokemon& player, const Pokemon& enemy);
    // 玩家使用技能，返回结算结果；无法行动时返回未命中结果
    DamageResult playerUseSkill(int skillIdx);
    // 敌方出招，返回实际技能索引（无技能/无法行动返回 -1），结果写入 result
    int enemyUseSkill(DamageResult* result = nullptr);
    void checkFaint();

private:
    // 睡眠等“行动前”状态；返回 false 表示本回合不能行动
    bool prepareAction(Pokemon& pokemon, QString& log);
    DamageResult useSkill(Pokemon& attacker, Pokemon& defender, const Skill& skill, QString& log);
    void applyStatusEffect(Pokemon& target, const Skill& skill, const DamageResult& res, QString& log);
    void applyEndTurnStatus(Pokemon& pokemon, QString& log);
    int chooseEnemySkill() const;
};

#endif // BATTLESTATE_H
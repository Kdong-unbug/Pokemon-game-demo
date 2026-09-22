#include "damagedeal.h"
#include <QRandomGenerator>
#include <QtGlobal>

DamageResult damagedeal::calculate(const Skill &skill, const Pokemon &attacker, const Pokemon &defender)
{
    DamageResult res;
    res.isHit = true;
    res.damage = 0;
    res.rate = 1.0;
    res.isCrit = false;
    res.appliedEffect = StatusEffect::None;

    // 命中判定
    if (skill.accuracy > 0) {
        int accuracy = qBound(1, skill.accuracy, 100);
        if (QRandomGenerator::global()->bounded(100) >= accuracy) {
            res.isHit = false;
            return res;
        }
    }

    // 属性克制倍率
    res.rate = damagedeal::getTypeRate(skill.skilltype, defender.elemType);

    // 纯状态类技能不造成伤害
    if (skill.power <= 0) {
        if (skill.effectChance > 0 &&
            defender.status == StatusEffect::None &&
            QRandomGenerator::global()->bounded(100) < skill.effectChance) {
            res.appliedEffect = static_cast<StatusEffect>(skill.effect);
        }
        return res;
    }

    // 适合小等级游戏的伤害公式，让 5~30 级战斗保持 2~5 回合
    double atk = attacker.attack + 5.0;
    double def = defender.defense + 5.0;
    double levelFactor = 0.25 + attacker.level * 0.03;
    double baseDamage = skill.power * (atk / def) * levelFactor;
    baseDamage *= res.rate;

    // 灼烧状态下攻击减半
    if (attacker.status == StatusEffect::Burn)
        baseDamage *= 0.5;

    // 随机波动 0.85 ~ 1.0
    baseDamage *= 0.85 + QRandomGenerator::global()->generateDouble() * 0.15;

    // 暴击
    if (skill.critRate > 0 &&
        QRandomGenerator::global()->generateDouble() < skill.critRate) {
        baseDamage *= 1.5;
        res.isCrit = true;
    }

    res.damage = qMax(1, static_cast<int>(baseDamage));

    // 附带异常状态
    if (skill.effectChance > 0 &&
        defender.status == StatusEffect::None &&
        QRandomGenerator::global()->bounded(100) < skill.effectChance) {
        res.appliedEffect = static_cast<StatusEffect>(skill.effect);
    }

    return res;
}

double damagedeal::getTypeRate(int skillType, int defType)
{
    // 0普通 1火 2水 3草
    if (skillType == 1 && defType == 3) return 2.0;   // 火克草
    if (skillType == 2 && defType == 1) return 2.0;   // 水克火
    if (skillType == 3 && defType == 2) return 2.0;   // 草克水

    if (skillType == 1 && defType == 2) return 0.5;   // 火打水不利
    if (skillType == 2 && defType == 3) return 0.5;   // 水打草不利
    if (skillType == 3 && defType == 1) return 0.5;   // 草打火不利

    return 1.0;
}

QString damagedeal::getStatusName(int effect)
{
    switch (static_cast<StatusEffect>(effect)) {
    case StatusEffect::Poison: return "中毒";
    case StatusEffect::Burn:   return "灼烧";
    case StatusEffect::Sleep:  return "睡眠";
    default:                   return "正常";
    }
}
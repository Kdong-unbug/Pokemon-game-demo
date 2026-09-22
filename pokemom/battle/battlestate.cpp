#include "battlestate.h"
#include <QRandomGenerator>
#include <QtGlobal>

battlestate::battlestate()
{
    state = BattleState::WAIT_INPUT;
    lastLog.clear();
}

void battlestate::setupBattle(const Pokemon& player, const Pokemon& enemy)
{
    playerPoke = player;
    enemyPoke = enemy;
    playerPoke.hp = qBound(0, playerPoke.hp, playerPoke.maxhp);
    enemyPoke.hp = qBound(0, enemyPoke.hp, enemyPoke.maxhp);
    state = BattleState::WAIT_INPUT;
    lastLog.clear();
}

DamageResult battlestate::playerUseSkill(int skillIdx)
{
    DamageResult noHit;
    noHit.isHit = false;
    noHit.damage = 0;
    noHit.rate = 1.0;
    noHit.isCrit = false;
    noHit.appliedEffect = StatusEffect::None;
    lastLog.clear();

    if (state != BattleState::WAIT_INPUT ||
        skillIdx < 0 || skillIdx >= playerPoke.skills.size()) {
        lastLog = QString("%1 无法行动！").arg(playerPoke.Pokemonname);
        return noHit;
    }

    QString log;
    DamageResult res = noHit;
    if (prepareAction(playerPoke, log))
        res = useSkill(playerPoke, enemyPoke, playerPoke.skills.at(skillIdx), log);

    lastLog = log;
    state = BattleState::RUN_TURN;
    checkFaint();
    return res;
}

int battlestate::enemyUseSkill(DamageResult *result)
{
    DamageResult noHit;
    noHit.isHit = false;
    noHit.damage = 0;
    noHit.rate = 1.0;
    noHit.isCrit = false;
    noHit.appliedEffect = StatusEffect::None;
    lastLog.clear();

    if (result)
        *result = noHit;

    if (enemyPoke.skills.isEmpty()) {
        lastLog = QString("%1 没有可用的技能！").arg(enemyPoke.Pokemonname);
        return -1;
    }

    QString log;
    int usedIdx = -1;
    DamageResult res = noHit;
    if (prepareAction(enemyPoke, log)) {
        usedIdx = chooseEnemySkill();
        res = useSkill(enemyPoke, playerPoke, enemyPoke.skills.at(usedIdx), log);
    }

    // 双方都行动完后结算中毒/灼烧
    applyEndTurnStatus(playerPoke, log);
    applyEndTurnStatus(enemyPoke, log);

    if (result)
        *result = res;
    lastLog = log;

    checkFaint();
    if (state == BattleState::RUN_TURN)
        state = BattleState::WAIT_INPUT;
    return usedIdx;
}

void battlestate::checkFaint()
{
    if (enemyPoke.hp <= 0) {
        enemyPoke.hp = 0;
        state = BattleState::WIN;
        return;
    }
    if (playerPoke.hp <= 0) {
        playerPoke.hp = 0;
        state = BattleState::LOSE;
        return;
    }
    state = BattleState::RUN_TURN;
}

bool battlestate::prepareAction(Pokemon &pokemon, QString &log)
{
    if (pokemon.status != StatusEffect::Sleep)
        return true;

    if (pokemon.statusTurns <= 0) {
        pokemon.status = StatusEffect::None;
        log += QString("%1 醒来了！").arg(pokemon.Pokemonname);
        return true;
    }

    --pokemon.statusTurns;
    log += QString("%1 正在沉睡，无法行动…").arg(pokemon.Pokemonname);
    if (pokemon.statusTurns <= 0) {
        pokemon.statusTurns = 0;
        pokemon.status = StatusEffect::None;
        log += QString("\n%1 醒来了！").arg(pokemon.Pokemonname);
    }
    return false;
}

DamageResult battlestate::useSkill(Pokemon &attacker, Pokemon &defender,
                                   const Skill &skill, QString &log)
{
    DamageResult res = damagedeal::calculate(skill, attacker, defender);
    log += QString("%1 使用了 %2！").arg(attacker.Pokemonname, skill.Skillname);

    if (!res.isHit) {
        log += QString("\n但是没有命中…");
        return res;
    }

    if (res.damage > 0) {
        defender.hp = qMax(0, defender.hp - res.damage);
        log += QString("\n造成 %1 点伤害").arg(res.damage);
    }

    if (res.isCrit)
        log += "，会心一击！";
    if (res.rate > 1.0)
        log += "\n效果拔群！";
    else if (res.rate < 1.0)
        log += "\n效果不太理想…";

    if (res.appliedEffect != StatusEffect::None)
        applyStatusEffect(defender, skill, res, log);

    return res;
}

void battlestate::applyStatusEffect(Pokemon &target, const Skill &skill,
                                    const DamageResult &res, QString &log)
{
    Q_UNUSED(skill);
    target.status = res.appliedEffect;
    if (res.appliedEffect == StatusEffect::Sleep)
        target.statusTurns = 1 + QRandomGenerator::global()->bounded(3); // 1~3 回合
    else
        target.statusTurns = 0;

    log += QString("\n%1 陷入%2！")
               .arg(target.Pokemonname, damagedeal::getStatusName(static_cast<int>(target.status)));
}

void battlestate::applyEndTurnStatus(Pokemon &pokemon, QString &log)
{
    if (pokemon.hp <= 0 || (pokemon.status != StatusEffect::Poison &&
                            pokemon.status != StatusEffect::Burn))
        return;

    int divisor = (pokemon.status == StatusEffect::Poison) ? 8 : 16;
    int dmg = qMax(1, pokemon.maxhp / divisor);
    pokemon.hp = qMax(0, pokemon.hp - dmg);
    log += QString("\n%1 因%2损失 %3 HP")
               .arg(pokemon.Pokemonname,
                    damagedeal::getStatusName(static_cast<int>(pokemon.status)))
               .arg(dmg);
}

int battlestate::chooseEnemySkill() const
{
    int bestIdx = 0;
    double bestScore = -1.0;
    for (int i = 0; i < enemyPoke.skills.size(); ++i) {
        const Skill& s = enemyPoke.skills.at(i);
        double typeRate = damagedeal::getTypeRate(s.skilltype, playerPoke.elemType);
        double score;
        if (s.power <= 0)
            score = 15.0 + s.effectChance * 0.3;   // 状态技能
        else
            score = s.power * (0.5 + 0.5 * typeRate) + s.effectChance * 0.2;
        if (score > bestScore) {
            bestScore = score;
            bestIdx = i;
        }
    }
    return bestIdx;
}
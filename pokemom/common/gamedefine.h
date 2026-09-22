#ifndef GAMEDEFINE_H
#define GAMEDEFINE_H

#include <QString>

//元素属性
enum class ElementType
{
    Normal = 0,
    Fire,//1
    Water,//2
    Grass//3
};

//异常状态
enum class StatusEffect
{
    None = 0,
    Poison,    //1中毒
    Burn,      //2灼烧
    Sleep      //3睡眠
};

//战斗状态机状态
enum class BattleState
{
    WAIT_INPUT,//等待玩家选择技能
    RUN_TURN,//回合开始进行
    CHECK_FAINT,//是否结束
    WIN,//获胜
    LOSE//失败
};

#endif // GAMEDEFINE_H

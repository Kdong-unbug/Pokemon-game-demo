#include "skill.h"

Skill::Skill()
{
  Skillname = "";
     power = 0;//威力
    skilltype = 0;//属性
    accuracy = 100;
     critRate = 0;//暴击概率
    effect = 0;//附带属性 1中毒2睡眠3灼烧
    effectChance = 0;
    priority = 0;
}

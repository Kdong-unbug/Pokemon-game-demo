#ifndef SKILL_H
#define SKILL_H
#include <QString>


class Skill
{

public:
        QString Skillname;//技能名
        int power;//威力
        int skilltype;//属性
        int accuracy;//命中率 0~100
        double critRate;//暴击概率
        int effect;//附带属性 1中毒2睡眠3灼烧
        int effectChance;//异常状态触发概率 0~100
        int priority;//先制度，数值越大越先出手


        Skill();

};

#endif // SKILL_H

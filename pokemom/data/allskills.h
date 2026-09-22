#ifndef ALLSKILLS_H
#define ALLSKILLS_H

#include "skill.h"
#include <QList>

//技能ID
enum class SkillId
{
    Spark,      //1火花
    Wrap,       //2缠绕
    WaterGun,   //3水枪
    Tackle,     //撞击
    QuickAttack,//电光一闪
    FireFang,   //火焰牙
    AquaJet,    //水流喷射
    VineWhip,   //藤鞭
    SleepPowder,//催眠粉（无伤害，高概率睡眠）
    BubbleBeam, //泡沫光线
    RazorLeaf,  //飞叶快刀
    FireWheel   //火焰轮
};

class AllSkills
{
public:
    //初始化技能库
    static void init();
    //按ID获取技能
    static Skill getSkill(SkillId id);
static void clear() { skillLib.clear(); }
private:
    static QList<Skill> skillLib;
};

#endif // ALLSKILLS_H

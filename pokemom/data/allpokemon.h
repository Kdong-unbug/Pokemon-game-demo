#ifndef ALLPOKEMON_H
#define ALLPOKEMON_H

#include "pokemon.h"
#include <QVector>

class allpokemon
{
public:
    static void initAllTemplate();
    static Pokemon getTemplate(int index);
    static Pokemon createInstance(int index, int level);
    static void ensureSpeciesSkills(Pokemon& pokemon); // 补齐/限制每个精灵的技能格
    static void clearTemplates() { m_templateList.clear(); }  // 添加


private:
    static QVector<Pokemon> m_templateList; //静态容器保存全部模板
};

#endif // ALLPOKEMON_H

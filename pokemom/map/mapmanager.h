#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QPoint>
#include <QRect>

// 单个 NPC 的数据
struct NpcData {
    QString name;               // 显示名称
    QString portraitPath;       // 头像图片路径（可为空）
    QPoint pos;                 // 在地图上的坐标 (x, y)
    QStringList dialogues;      // 对话内容（多段）
};

// 单张地图的数据
struct MapData {
    QString backgroundPath;     // 背景图片路径
    QVector<NpcData> npcs;      // 该地图的所有 NPC
    QPoint playerStartPos;      // 玩家进入时的初始位置（可选）
    QVector<QRect> obstacles;   // 新增：不可通行的矩形区域
};

class MapManager
{
public:
    MapManager();

    // 初始化所有地图数据
    void initMaps();

    // 获取当前地图数据
    const MapData& currentMap() const;

    // 获取当前地图索引
    int currentIndex() const;

    // 地图切换（水平方向）
    bool moveLeft();
    bool moveRight();
    bool moveUp();      // 暂未实现，可扩展
    bool moveDown();    // 暂未实现，可扩展

    // 直接切换到指定索引
    bool switchTo(int index);

    // 计算玩家进入新地图时的位置（根据离开方向）
    QPoint getEntryPosition(int fromIndex, int toIndex) const;

private:
    QVector<MapData> m_maps;
    int m_currentIndex;
};

#endif // MAPMANAGER_H

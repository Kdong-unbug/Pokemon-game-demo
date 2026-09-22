#include "mapmanager.h"

MapManager::MapManager()
    : m_currentIndex(0)
{
    initMaps();
}

void MapManager::initMaps()
{
    // ============================================================
    // 在这里配置所有地图数据
    // 每个地图包含：背景、NPC列表、玩家出生点、障碍物矩形
    // ============================================================

    m_maps = {
        // ---------- 地图0 ----------
        {
            ":/new/prefix1/pic/map1.png",   // 背景
            {   // NPC 列表
                {
                    "J博士",
                    ":/new/prefix1/pic/MS.J.png",
                    QPoint(500, 300),
                    {"年轻人你好！我是J博士。",
                     "我正在研究宝可梦的进化奥秘...",
                     "这里有一只野生的小火狐，请你挑选一个你喜欢的精灵和他对战吧。"}
                },
                {
                    "路人甲",
                    ":/new/prefix1/pic/npc1.png",
                    QPoint(800, 600),
                    {"你是新来的吧！","快去博士那里领取自己的第一只精灵吧!"}
                }
            },
            QPoint(100, 500),    // 玩家起始位置
            {   // 障碍物矩形
                QRect(570, 314, 20,55)

            }
        },

        // ---------- 地图1 ----------
        {
            ":/new/prefix1/pic/map2.png",
            { },
            QPoint(1100, 500),
            {   // 障碍物

            }
        },

        // ---------- 地图2 ----------
        {
            ":/new/prefix1/pic/map3.png",
            {
                {
                    "商人",
                    ":/new/prefix1/pic/shopnpc.png",
                    QPoint(900, 780),
                    {"欢迎光临！", "想买点什么？"}
                },
                {
                    "对战管理员",
                    ":/new/prefix1/pic/fightnpc.png",
                    QPoint(1000, 470),
                    {"你好训练师！", "想要参加比赛吗？"}
                }
            },
            QPoint(600, 100),
            {   // 障碍物


            }
        }
    };

    m_currentIndex = 0;
}

const MapData& MapManager::currentMap() const
{
    return m_maps[m_currentIndex];
}

int MapManager::currentIndex() const
{
    return m_currentIndex;
}

bool MapManager::moveLeft()
{
    if (m_currentIndex > 0)
    {
        --m_currentIndex;
        return true;
    }
    return false;
}

bool MapManager::moveRight()
{
    if (m_currentIndex < m_maps.size() - 1)
    {
        ++m_currentIndex;
        return true;
    }
    return false;
}

bool MapManager::moveUp()
{
    // 暂未实现垂直切换
    return false;
}

bool MapManager::moveDown()
{
    return false;
}

bool MapManager::switchTo(int index)
{
    if (index >= 0 && index < m_maps.size())
    {
        m_currentIndex = index;
        return true;
    }
    return false;
}

QPoint MapManager::getEntryPosition(int fromIndex, int toIndex) const
{
    // 根据方向决定玩家出现在新地图的哪一侧
    if (fromIndex > toIndex)
    {
        // 向左走，出现在右侧
        return QPoint(1200, 500);   // 假设窗口宽1260，让玩家靠右
    } else if (fromIndex < toIndex)
    {
        // 向右走，出现在左侧
        return QPoint(50, 500);
    }
    // 默认返回起始位置
    return m_maps[toIndex].playerStartPos;
}

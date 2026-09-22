#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>
#include <QDialog>
#include <QPushButton>
#include <QVector>
#include <QPointer>
#include "player.h"
#include "npcdialog.h"
#include "pokemoninfo.h"
#include "mapmanager.h"
#include "wildencounter.h"
#include "netbattlehost.h"
#include "netbattleclient.h"
#include "netfightwindow.h"


class BattleWindow;

namespace Ui { class mainwindow; }

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr);
    ~mainwindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void movePlayer();
    void showPokemonSelection();
    void selectPokemon(int index);
    void openBag();


private:
    Ui::mainwindow *ui;

    // ===== 地图相关 =====
    QLabel *mapLabel;                 // 背景地图标签
    MapManager *m_mapManager;         // 地图管理器
    QVector<QLabel*> m_npcLabels;     // 当前地图的所有 NPC 标签（动态创建）

    // ===== 玩家 =====
    QLabel *player;
    QTimer *timer;
    QPixmap pixLeft, pixRight, pixUp, pixDown;
    int dx = 0;
    int dy = 0;
    int speed = 4;

    // ===== 玩家数据 =====
    Player* m_player = nullptr;

    // ===== 精灵选择对话框 =====
    QDialog* chooseDialog = nullptr;

    // ===== NPC交互 =====
    int findNearestNpcIndex();            // 检测最近的 NPC 索引，若无则返回 -1
    void showNpcDialog(int npcIndex);     // 显示指定 NPC 的对话
    void showNpcFirstDialog();            // 保留原入口（可选）
    void startBattle(Pokemon playerPoke); // 开始战斗

    // ===== 地图UI更新 =====
    void updateMapUI();                   // 刷新背景和 NPC 标签

    // ===== 背包窗口 =====
    QPointer<BagWindow> m_bagWindow;

    // ===== 存档 =====
    void saveGame();
    void loadGame();
    QString getSaveFilePath() const;

    // ===== 障碍物碰撞 =====
    bool isBlocked(int newX, int newY);          // 检测某坐标是否被障碍物阻挡
    void adjustPlayerPosition();


    //=======野生精灵对战=====
private:
    bool m_inBattle = false;                     // 是否战斗中
    int m_encounterStep = 0;                     // 移动步数，用于控制遇敌频率
    WildEncounterManager* m_wildManager = nullptr; // 野外遇敌规则

    void checkEncounter();                       // 检查遇敌
    void startWildBattle(const Pokemon& wild);   // 开始野生战斗



    //======网络对战======
    NetBattleHost* m_host = nullptr;
       NetBattleClient* m_client = nullptr;
       QDialog* m_waitDialog = nullptr;
       Pokemon m_myPokemon;
    void onNetworkBattleMenu();
    void startHost();
    void joinHost();



};

#endif // MAINWINDOW_H

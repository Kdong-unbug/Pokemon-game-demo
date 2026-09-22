#ifndef FIGHTWINDOW_H
#define FIGHTWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QDialog>
#include "battlestate.h"
#include "allskills.h"
#include "player.h"

class QCloseEvent;

class FightWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FightWindow(Player* player, const Pokemon& enemy, QWidget *parent = nullptr);
    ~FightWindow();

signals:
    void finished(bool win);
    void captureSuccess(const Pokemon& captured); // 捕捉成功信号，用于通知主窗口

protected:
    // 将技能点击槽改为虚函数，供子类重写（网络对战）
    virtual void onSkill(int index);
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onFight();
    void onBag();
    void onRun();
    void onEnemyAction();
    void updateStatus();
    void onEnd();
    void onBack();
    void onUseItem(int itemId);

public:
    void setupUI();
    void refresh();
    void showMenu();
    void showSkills();
    void setTurn(bool turn);
    void log(const QString& msg);
    void showItemMenu();
    void tryCapture();
    void syncPlayerPokemon();

    // UI
    QLabel* m_playerImg;
    QLabel* m_enemyImg;
    QLabel* m_playerName;
    QLabel* m_enemyName;
    QProgressBar* m_playerHp;
    QProgressBar* m_enemyHp;
    QLabel* m_playerHpText;
    QLabel* m_enemyHpText;
    QLabel* m_logLabel;

    QPushButton* m_btnFight;
    QPushButton* m_btnBag;
    QPushButton* m_btnRun;
    QPushButton* m_btnBack;
    QVector<QPushButton*> m_skillBtns;

    QWidget* m_menuPanel;
    QWidget* m_skillPanel;

    // 战斗立绘缓存（避免每次刷新都从资源重复解码）
    QPixmap m_playerPix;
    QPixmap m_enemyPix;
    QPixmap m_playerPixSmall;
    QPixmap m_enemyPixSmall;

    // 战斗
    battlestate* m_battle;
    Pokemon m_playerPoke;    // 玩家当前出战精灵副本（战斗中使用）
    Pokemon m_enemy;
    QTimer* m_timer;
    bool m_isPlayerTurn;
    bool m_isEnd;
    bool m_captured = false;

    Player* m_player;        // 玩家数据指针（用于访问背包和队伍）
};

#endif // FIGHTWINDOW_H

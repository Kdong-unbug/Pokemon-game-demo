#ifndef NETFIGHTWINDOW_H
#define NETFIGHTWINDOW_H

#include "fightwindow.h"
#include "netbattleclient.h"
#include "netbattlehost.h"
#include "battlestate.h"

// 网络对战窗口，继承自 FightWindow，但覆盖战斗逻辑
class NetFightWindow : public FightWindow
{
    Q_OBJECT
public:
    // 构造函数：
    // - isHost: true 表示本机是主机（拥有战斗权威），false 表示客户端
    // - hostPtr: 如果是主机，传入 NetBattleHost 指针（用于广播）
    // - clientPtr: 如果是客户端，传入 NetBattleClient 指针（用于发送/接收）
    explicit NetFightWindow(Player* player, const Pokemon& enemy,
                            bool isHost,
                            NetBattleHost* hostPtr = nullptr,
                            NetBattleClient* clientPtr = nullptr,
                            QWidget *parent = nullptr);
    ~NetFightWindow();

protected:
    // 重写技能点击事件：发送网络消息或本地处理
    void onSkill(int index) override;

private slots:
    void onNetworkMessage(const QJsonObject &obj);
    void onOpponentDisconnected();

private:
    bool m_isHost;
    NetBattleHost* m_host;
    NetBattleClient* m_client;

    // 辅助函数
    void applyTurnResult(const QJsonObject &result);
    void endBattle(bool win);
    void broadcastTurnResult(const QString& log, int hostHp, int clientHp);
};

#endif // NETFIGHTWINDOW_H

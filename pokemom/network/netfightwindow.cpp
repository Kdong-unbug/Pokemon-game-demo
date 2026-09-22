#include "netfightwindow.h"
#include <QMessageBox>
#include <QTimer>
#include <QJsonDocument>
#include "damagedeal.h"
#include "gamedefine.h"

NetFightWindow::NetFightWindow(Player* player, const Pokemon& enemy,
                               bool isHost,
                               NetBattleHost* hostPtr,
                               NetBattleClient* clientPtr,
                               QWidget *parent)
    : FightWindow(player, enemy, parent),
      m_isHost(isHost),
      m_host(hostPtr),
      m_client(clientPtr)
{
    // 停止基类定时器（网络对战不使用自动刷新）
    if (m_timer) {
        m_timer->stop();
        disconnect(m_timer, nullptr, this, nullptr);
        delete m_timer;
        m_timer = nullptr;
    }

    // 删除基类创建的 battlestate（我们不用）
    if (m_battle) {
        delete m_battle;
        m_battle = nullptr;
    }

    // 初始回合：主机先手
    if (m_isHost) {
        setTurn(true);
        log("你的回合，选择技能！");
    } else {
        setTurn(false);
        log("等待对手行动...");
    }

    // 连接网络信号
    if (m_client) {
        connect(m_client, &NetBattleClient::jsonReceived, this, &NetFightWindow::onNetworkMessage);
        connect(m_client, &NetBattleClient::disconnected, this, &NetFightWindow::onOpponentDisconnected);
    }
    if (m_host) {
        connect(m_host, &NetBattleHost::jsonReceived, this,
                [this](QTcpSocket*, const QJsonObject &obj) {
                    onNetworkMessage(obj);
                });
        connect(m_host, &NetBattleHost::clientDisconnected, this,
                [this](QTcpSocket*) {
                    onOpponentDisconnected();
                });
    }

    // 禁用道具和逃跑（网络对战暂不支持）
    if (m_btnBag) m_btnBag->setEnabled(false);
    if (m_btnRun) m_btnRun->setEnabled(false);
}

NetFightWindow::~NetFightWindow()
{
    // 基类会清理 m_timer 和 m_battle，我们已经置空
}

void NetFightWindow::onSkill(int index)
{
    if (m_isEnd) return;

    if (m_isHost) {
        // ---- 主机使用技能（攻击客户端的精灵） ----
        if (!m_isPlayerTurn) return;
        if (index < 0 || index >= m_playerPoke.skills.size()) return;

        Skill& skill = m_playerPoke.skills[index];
        DamageResult res = damagedeal::calculate(skill, m_playerPoke, m_enemy);
        if (res.isHit) {
            m_enemy.hp -= res.damage;
            if (m_enemy.hp < 0) m_enemy.hp = 0;
        }
        refresh();

        QString logMsg = QString("%1 使用了 %2！").arg(m_playerPoke.Pokemonname).arg(skill.Skillname);
        log(logMsg);

        // 广播结果（主机HP是 m_playerPoke.hp，客户端HP是 m_enemy.hp）
        broadcastTurnResult(logMsg, m_playerPoke.hp, m_enemy.hp);

        // 检查胜负
        if (m_enemy.hp <= 0) {
            endBattle(true);
            return;
        }

        // 轮到客户端
        setTurn(false);
        QJsonObject turnMsg;
        turnMsg["type"] = "your_turn";
        m_host->broadcast(turnMsg);
    }
    else {
        // ---- 客户端发送技能选择 ----
        if (!m_isPlayerTurn) return;
        if (index < 0 || index >= m_playerPoke.skills.size()) return;

        QJsonObject msg;
        msg["type"] = "skill";
        msg["index"] = index;
        m_client->sendJson(msg);
        setTurn(false);
        log("等待对手回应...");
    }
}

void NetFightWindow::onNetworkMessage(const QJsonObject &obj)
{
    QString type = obj["type"].toString();

    if (type == "skill") {
        // 只有主机才会收到客户端的技能选择
        if (!m_isHost) return;

        int idx = obj["index"].toInt();
        if (idx < 0 || idx >= m_enemy.skills.size()) return;

        // 客户端精灵（敌方）攻击主机精灵
        Skill& skill = m_enemy.skills[idx];
        DamageResult res = damagedeal::calculate(skill, m_enemy, m_playerPoke);
        if (res.isHit) {
            m_playerPoke.hp -= res.damage;
            if (m_playerPoke.hp < 0) m_playerPoke.hp = 0;
        }
        refresh();

        QString logMsg = QString("%1 使用了 %2！").arg(m_enemy.Pokemonname).arg(skill.Skillname);
        log(logMsg);

        // 广播结果
        broadcastTurnResult(logMsg, m_playerPoke.hp, m_enemy.hp);

        if (m_playerPoke.hp <= 0) {
            endBattle(false);
            return;
        }

        // 轮到主机
        setTurn(true);
        log("你的回合，选择技能！");
        // 可广播一个 your_turn 给客户端（但客户端已在等待，不必重复发送）
    }
    else if (type == "turn_result") {
        // 所有玩家都接收回合结果
        applyTurnResult(obj);
    }
    else if (type == "your_turn") {
        // 只有客户端收到该消息才启用操作
        if (!m_isHost) {
            setTurn(true);
            log("你的回合，选择技能！");
        }
    }
    else if (type == "battle_end") {
        // 广播里的 win 表示“主机是否获胜”，客户端需要反转后再显示
        bool hostWin = obj["win"].toBool();
        endBattle(m_isHost ? hostWin : !hostWin);
    }
}

void NetFightWindow::onOpponentDisconnected()
{
    QMessageBox::warning(this, "连接断开", "对手已断开连接！");
    m_isEnd = true;
    setTurn(false);
    if (m_timer)
        m_timer->stop();
    syncPlayerPokemon();
    emit finished(false);
    close();
}

void NetFightWindow::applyTurnResult(const QJsonObject &result)
{
    int hostHp = result["hostHp"].toInt();
    int clientHp = result["clientHp"].toInt();

    if (m_isHost) {
        m_playerPoke.hp = hostHp;
        m_enemy.hp = clientHp;
    } else {
        m_playerPoke.hp = clientHp;
        m_enemy.hp = hostHp;
    }
    refresh();
    log(result["log"].toString());
}

void NetFightWindow::broadcastTurnResult(const QString& logMsg, int hostHp, int clientHp)
{
    QJsonObject result;
    result["type"] = "turn_result";
    result["hostHp"] = hostHp;
    result["clientHp"] = clientHp;
    result["log"] = logMsg;
    m_host->broadcast(result);
}

void NetFightWindow::endBattle(bool win)
{
    if (m_isEnd) return;
    m_isEnd = true;
    setTurn(false);
    log(win ? "胜利！" : "失败...");

    // 把本机出战精灵的最终状态同步回玩家队伍
    if (m_player &&
        m_player->currentPokemonIndex >= 0 &&
        m_player->currentPokemonIndex < m_player->team.size()) {
        m_player->team[m_player->currentPokemonIndex] = m_playerPoke;
    }

    if (m_isHost) {
        QJsonObject endMsg;
        endMsg["type"] = "battle_end";
        endMsg["win"] = win;
        m_host->broadcast(endMsg);
    }

    QTimer::singleShot(2000, this, [this, win]() {
        emit finished(win);
        close();
    });
}

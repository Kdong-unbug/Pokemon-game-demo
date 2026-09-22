#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "npcdialog.h"
#include "player.h"
#include "allpokemon.h"
#include "allskills.h"
#include "battlestate.h"
#include <QDebug>
#include "fightwindow.h"
#include "pokemoninfo.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QInputDialog>
#include <QRandomGenerator>
#include "shopwindow.h"

// 全局对话组件
NpcDialog *npcDialog = nullptr;

mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainwindow)
    , chooseDialog(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("宝可梦");
    resize(1260, 1000);

    // ==========================================
    // 1. 创建玩家
    // ==========================================
    m_player = new Player();

    // ==========================================
    // 2. 初始化地图管理器
    // ==========================================
    m_mapManager = new MapManager();
    m_wildManager = new WildEncounterManager();

    // ==========================================
    // 3. UI设置
    // ==========================================
    // 背景地图标签
    mapLabel = new QLabel(this);
    mapLabel->setGeometry(0, 0, 1260, 1000);
    mapLabel->lower();

    // 玩家方向图
    pixLeft  = QPixmap(":/new/prefix1/pic/play2.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixRight = QPixmap(":/new/prefix1/pic/play3.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixUp    = QPixmap(":/new/prefix1/pic/play0.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixDown  = QPixmap(":/new/prefix1/pic/play1.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建玩家
    player = new QLabel(this);
    player->setPixmap(pixDown);
    player->setGeometry(500, 500, 128, 128);

    // 移动定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &mainwindow::movePlayer);
    timer->start(50);

    // 创建全局对话组件
    npcDialog = new NpcDialog(this);

    // ==========================================
    // 加载第一张地图
    // ==========================================
    updateMapUI();
    // 确保玩家初始位置合法
    adjustPlayerPosition();



}

// ==========================================
// 更新地图UI
// ==========================================
void mainwindow::updateMapUI()
{
    if (!m_mapManager) return;
    const MapData& data = m_mapManager->currentMap();

    // ----- 更新背景 -----
    QPixmap bg(data.backgroundPath);
    if (!bg.isNull())
        mapLabel->setPixmap(bg.scaled(1260, 1000, Qt::KeepAspectRatioByExpanding));

    // ----- 清除旧的 NPC 标签 -----
    for (auto label : m_npcLabels) {
        delete label;
    }
    m_npcLabels.clear();

    // ----- 创建新的 NPC 标签 -----
    for (const NpcData& npc : data.npcs)
    {
        QLabel* label = new QLabel(this);
        if (!npc.portraitPath.isEmpty())
        {
            QPixmap pix(npc.portraitPath);
            if (!pix.isNull())
                label->setPixmap(pix.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            else
                label->setText(npc.name);  // 图片加载失败时显示名称
        } else
        {
            label->setText(npc.name);
        }
        label->setGeometry(npc.pos.x(), npc.pos.y(), 128, 128);
        label->setToolTip(npc.name);
        label->show();
        m_npcLabels.append(label);
    }
}

// ==========================================
// 检测最近的 NPC
// ==========================================
int mainwindow::findNearestNpcIndex()
{
    int px = player->x() + player->width() / 2;
    int py = player->y() + player->height() / 2;
    const int threshold = 150;

    for (int i = 0; i < m_npcLabels.size(); ++i)
    {
        QLabel* npc = m_npcLabels[i];
        int nx = npc->x() + npc->width() / 2;
        int ny = npc->y() + npc->height() / 2;
        int distSq = (px - nx)*(px - nx) + (py - ny)*(py - ny);
        if (distSq < threshold * threshold) {
            return i;
        }
    }
    return -1;
}

// ==========================================
// 显示指定 NPC 的对话
// ==========================================
void mainwindow::showNpcDialog(int npcIndex)
{
    const QVector<NpcData>& npcs = m_mapManager->currentMap().npcs;
    if (npcIndex < 0 || npcIndex >= npcs.size()) return;

    const NpcData& npc = npcs[npcIndex];
    npcDialog->setName(npc.name);
    npcDialog->setLines(npc.dialogues);

    // 对话结束时的回调
    npcDialog->setOnFinished([this, npc]()
    {
        // 如果是 J博士 且玩家队伍为空，则显示精灵选择界面
        if (npc.name == "J博士" && m_player->getTeamSize() == 0)
        {
            showPokemonSelection();
        }
        else if(npc.name == "对战管理员")
        {
            onNetworkBattleMenu();
        }
        else if (npc.name == "商人")
            {
                ShopWindow* shop = new ShopWindow(m_player, this);
                shop->setAttribute(Qt::WA_DeleteOnClose);
                shop->exec();
            }
    });

    npcDialog->open();
}

// ==========================================
// 障碍物碰撞检测
// ==========================================
bool mainwindow::isBlocked(int newX, int newY)
{
    // 玩家矩形
    QRect playerRect(newX + 20, newY + 20,
                     player->width() - 40, player->height() - 40);
    // 获取当前地图的障碍物列表
    const MapData& map = m_mapManager->currentMap();
    for (const QRect& obstacle : map.obstacles)
    {
        if (playerRect.intersects(obstacle))
        {
            return true;
        }
    }
    return false;
}

// ==========================================
// 调整玩家位置，确保不在障碍物内
// ==========================================
void mainwindow::adjustPlayerPosition()
{
    int x = player->x();
    int y = player->y();

    // 如果当前位置被阻塞，尝试使用地图起始位置
    if (isBlocked(x, y))
    {
        QPoint start = m_mapManager->currentMap().playerStartPos;
        player->move(start);
        // 如果起始位置也被阻塞（配置错误），强行移到 (10,10) 安全区
        if (isBlocked(player->x(), player->y())) {
            player->move(10, 10);
        }
    }
}

// ==========================================
// 玩家移动（支持地图切换 + 障碍物检测）
// ==========================================
void mainwindow::movePlayer()
{
    if (dx == 0 && dy == 0) return;

    int x = player->x();
    int y = player->y();
    int w = player->width();
    int h = player->height();
    int winW = centralWidget()->width();
    int winH = centralWidget()->height();

    int newX = x + dx * speed;
    int newY = y + dy * speed;

    bool needSwitch = false;
    int oldIndex = m_mapManager->currentIndex();
    int newIndex = oldIndex;
    QPoint entryPos = player->pos();

    // 水平越界检测（地图切换）
    if (newX < 0) {
        if (m_mapManager->moveLeft()) {
            needSwitch = true;
            newIndex = m_mapManager->currentIndex();
            newX = winW - w;   // 从右侧进入
        } else {
            newX = 0;
        }
    } else if (newX + w > winW)

    {
        if (m_mapManager->moveRight())
        {
            needSwitch = true;
            newIndex = m_mapManager->currentIndex();
            newX = 0;          // 从左侧进入
        } else {
            newX = winW - w;
        }
    }

    // 上下边界（暂不切换）
    if (!needSwitch) {
        if (newY < 0) newY = 0;
        else if (newY + h > winH) newY = winH - h;
    }

    // ----- 如果发生地图切换 -----
    if (needSwitch)
    {
        // 更新地图UI（背景和NPC）
        updateMapUI();

        // 计算入口位置（左右方向）
        if (oldIndex > newIndex) { // 向左走
            entryPos = QPoint(winW - w, newY);
        } else {                   // 向右走
            entryPos = QPoint(0, newY);
        }



        player->move(entryPos);
        // 最后再微调一次，确保安全
        adjustPlayerPosition();

    } else {
        // ----- 未切换地图，正常移动（需要碰撞检测） -----
        if (!isBlocked(newX, newY))
        {
            player->move(newX, newY);
            // 只有在真正移动后才检查遇敌
            checkEncounter();
        }
        // 若碰撞则原地不动
    }
}

// ==========================================
// 按键事件
// ==========================================
void mainwindow::openBag()
{
    if (m_bagWindow) {
        m_bagWindow->raise();
        m_bagWindow->activateWindow();
        return;
    }

    m_bagWindow = new BagWindow(m_player, this);
    m_bagWindow->setAttribute(Qt::WA_DeleteOnClose);
    m_bagWindow->show();
    m_bagWindow->raise();
    m_bagWindow->activateWindow();
}

void mainwindow::keyPressEvent(QKeyEvent *event)
{
    // B键打开背包
    if (event->key() == Qt::Key_B)
    {
        openBag();
        return;
    }
    if (event->key() == Qt::Key_F5)
    {
        saveGame();
        return;
    }
    // 按 F9 读档
    if (event->key() == Qt::Key_F9)
    {
        loadGame();
        return;
    }

    // 如果对话打开，交给对话处理
    if (npcDialog && npcDialog->isOpen())
    {
        npcDialog->handleKeyPress(event);
        return;
    }

    // 方向键
    switch (event->key())
    {
        case Qt::Key_W:
            dy = -speed;
            player->setPixmap(pixUp);
            break;
        case Qt::Key_S:
            dy = speed;
            player->setPixmap(pixDown);
            break;
        case Qt::Key_A:
            dx = -speed;
            player->setPixmap(pixLeft);
            break;
        case Qt::Key_D:
            dx = speed;
            player->setPixmap(pixRight);
            break;
        case Qt::Key_E:
            {
                int idx = findNearestNpcIndex();
                if (idx != -1) {
                    showNpcDialog(idx);
                }
            }
            break;
        default:
            QMainWindow::keyPressEvent(event);
    }
}

void mainwindow::keyReleaseEvent(QKeyEvent *event)
{
    if (npcDialog && npcDialog->isOpen()) return;

    switch (event->key()) {
        case Qt::Key_W:
            if (dy < 0) dy = 0;
            break;
        case Qt::Key_S:
            if (dy > 0) dy = 0;
            break;
        case Qt::Key_A:
            if (dx < 0) dx = 0;
            break;
        case Qt::Key_D:
            if (dx > 0) dx = 0;
            break;
        default:
            QMainWindow::keyReleaseEvent(event);

    }
}

// ==========================================
// 精灵选择界面
// ==========================================
void mainwindow::showPokemonSelection()
{
    if (chooseDialog) {
        delete chooseDialog;
    }
    chooseDialog = new QDialog(this);
    chooseDialog->setWindowTitle("选择你的宝可梦");
    chooseDialog->setModal(true);
    chooseDialog->resize(1260, 1000);
    chooseDialog->setStyleSheet(
        "QDialog {"
        "   border-image: url(:/new/prefix1/pic/first.png) stretch;"
        "}"
    );

    QString hiddenBtnStyle = R"(
        QPushButton {
            background: transparent;
            border: none;
            outline: none;
        }
        QPushButton:hover {
            background: rgba(255, 255, 255, 30);
        }
        QPushButton:pressed {
            background: rgba(255, 255, 255, 60);
        }
    )";

    // 火系
    QPushButton* btn_fire = new QPushButton(chooseDialog);
    btn_fire->setGeometry(260, 380, 120, 120);
    btn_fire->raise();
    btn_fire->setStyleSheet(hiddenBtnStyle);
    btn_fire->setToolTip("选择火系宝可梦：小火狐");
    connect(btn_fire, &QPushButton::clicked, this, [this]()
    {
        selectPokemon(0);
    });

    // 水系
    QPushButton* btn_water = new QPushButton(chooseDialog);
    btn_water->setGeometry(880, 380, 120, 120);
    btn_water->raise();
    btn_water->setStyleSheet(hiddenBtnStyle);
    btn_water->setToolTip("选择水系宝可梦：小水獭");
    connect(btn_water, &QPushButton::clicked, this, [this]()
    {
        selectPokemon(1);
    });

    // 草系
    QPushButton* btn_grass = new QPushButton(chooseDialog);
    btn_grass->setGeometry(590, 540, 120, 120);
    btn_grass->raise();
    btn_grass->setStyleSheet(hiddenBtnStyle);
    btn_grass->setToolTip("选择草系宝可梦：菌菇鹿");
    connect(btn_grass, &QPushButton::clicked, this, [this]()
    {
        selectPokemon(2);
    });

    chooseDialog->exec();
}

// ==========================================
// 选择精灵
// ==========================================
void mainwindow::selectPokemon(int index)
{
    Pokemon selectedPoke = allpokemon::createInstance(index, 5);

    m_player->team.clear();
    m_player->addPokemon(selectedPoke);
    m_player->setCurrentPokemon(0);

    if (chooseDialog)
    {
        chooseDialog->accept();
    }

    if (npcDialog && npcDialog->isOpen())
    {
        npcDialog->close();
    }

    startBattle(selectedPoke);
}

// ==========================================
// 开始战斗
// ==========================================
void mainwindow::startBattle(Pokemon playerPoke)
{
    // 博士的指导战：对手是 5 级小火狐
    Pokemon wildPoke = allpokemon::createInstance(0, 5);
    if (playerPoke.skills.isEmpty())
        playerPoke.skills.append(AllSkills::getSkill(SkillId::Tackle));
    if (wildPoke.skills.isEmpty())
        wildPoke.skills.append(AllSkills::getSkill(SkillId::Spark));

    // 战斗期间暂停地图移动，避免角色在战斗时继续走动
    timer->stop();
    dx = 0;
    dy = 0;
    m_inBattle = true;

    FightWindow* fight = new FightWindow(m_player, wildPoke, this);
    fight->setAttribute(Qt::WA_DeleteOnClose);

    connect(fight, &FightWindow::finished, this, [=](bool win)
    {
        m_inBattle = false;
        timer->start();

        // 全队都倒下时自动送精灵中心恢复，避免游戏卡死
        if (!m_player->hasHealthyPokemon()) {
            m_player->healAll();
            QMessageBox::information(this, "精灵中心", "你的宝可梦都累倒了，已送精灵中心恢复完毕！");
        }

        if (npcDialog)
        {
            npcDialog->setName("J博士");
            if (win) {
                npcDialog->setLines({"太厉害了！你赢了！"});
            } else {
                npcDialog->setLines({"别灰心，继续加油！"});
            }
            npcDialog->setOnFinished(nullptr);
            npcDialog->open();
        }
    });

    fight->show();
}

// ==========================================
// 存档函数
// ==========================================
QString mainwindow::getSaveFilePath() const
{
    // 获取exe所在目录
    QDir exeDir(QCoreApplication::applicationDirPath());
    QString savePath = exeDir.filePath("PokemonSave.json");
    qDebug() << "存档路径:" << savePath;
    return savePath;
}


void mainwindow::saveGame()
{
    if (!m_player) return;

    QJsonObject root;
    root["player"] = m_player->toJson();
    root["mapIndex"] = m_mapManager->currentIndex();
    root["playerX"] = player->x();
    root["playerY"] = player->y();

    QFile file(getSaveFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "存档失败", "无法创建存档文件！");
        return;
    }
    file.write(QJsonDocument(root).toJson());
    file.close();
    QMessageBox::information(this, "存档成功", "游戏已保存！");
}

void mainwindow::loadGame()
{
    QFile file(getSaveFilePath());
    if (!file.exists()) {
        QMessageBox::information(this, "读档", "没有找到存档文件。");
        return;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "读档失败", "无法打开存档文件！");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        QMessageBox::warning(this, "读档失败", "存档文件损坏！");
        return;
    }
    QJsonObject root = doc.object();

    // 恢复玩家数据
    if (root.contains("player")) {
        m_player->fromJson(root["player"].toObject());
    }

    // 恢复地图
    int mapIndex = root["mapIndex"].toInt();
    if (m_mapManager->switchTo(mapIndex))
    {
        updateMapUI();
    }

    // 恢复玩家位置
    int px = root["playerX"].toInt(500);
    int py = root["playerY"].toInt(500);
    player->move(px, py);
    // 确保位置合法
    adjustPlayerPosition();

    // 如果背包窗口开着，关闭它
    if (m_bagWindow)
    {
        m_bagWindow->close();
        m_bagWindow = nullptr;
    }

    QMessageBox::information(this, "读档成功", "游戏已加载！");
}
// ==========================================
// 网络对战实现
// ==========================================


void mainwindow::onNetworkBattleMenu()
{
    if (m_player->getTeamSize() == 0)
    {
        QMessageBox::warning(this, "提示", "你还没有宝可梦，请先找J博士领取！");
        return;
    }
    m_myPokemon = m_player->getCurrentPokemon();

    QDialog menu(this);
    menu.setWindowTitle("网络对战");
    QVBoxLayout* layout = new QVBoxLayout(&menu);
    QPushButton* btnHost = new QPushButton("开房间（作为主机）");
    QPushButton* btnJoin = new QPushButton("加入房间（作为客户端）");
    QPushButton* btnCancel = new QPushButton("取消");

    connect(btnHost, &QPushButton::clicked, [&]() { menu.accept(); startHost(); });
    connect(btnJoin, &QPushButton::clicked, [&]() { menu.accept(); joinHost(); });
    connect(btnCancel, &QPushButton::clicked, &menu, &QDialog::reject);

    layout->addWidget(btnHost);
    layout->addWidget(btnJoin);
    layout->addWidget(btnCancel);
    menu.exec();
}


void mainwindow::startHost()
{
    // 清理旧 host（如果有）
    if (m_host) {
        m_host->close();
        delete m_host;
        m_host = nullptr;
    }

    m_host = new NetBattleHost(this);
    if (!m_host->startHost(8000)) {
        qDebug() << "[Host] 启动失败:" << m_host->errorString();
        QMessageBox::critical(this, "错误",
            "端口 8000 被占用或无法绑定，请关闭占用程序。\n" + m_host->errorString());
        delete m_host;
        m_host = nullptr;
        return;
    }
    qDebug() << "[Host] 监听状态:" << m_host->isListening() << "端口:" << m_host->serverPort();

    // 创建等待对话框
    m_waitDialog = new QDialog(this);
    m_waitDialog->setWindowTitle("等待对手");
    QLabel* label = new QLabel("正在等待对手连接...\n请将您的IP地址告诉对手");
    QPushButton* cancelBtn = new QPushButton("取消");
    connect(cancelBtn, &QPushButton::clicked, [this]() {
        m_host->close();
        delete m_host;
        m_host = nullptr;
        m_waitDialog->accept();
    });
    QVBoxLayout* lay = new QVBoxLayout(m_waitDialog);
    lay->addWidget(label);
    lay->addWidget(cancelBtn);

    // 当有客户端连接时，发送我方精灵
    connect(m_host, &NetBattleHost::clientConnected, this, [this](QTcpSocket* client) {
        qDebug() << "[Host] 客户端已连接，发送 init 消息";
        QJsonObject joinMsg;
        joinMsg["type"] = "init";
        joinMsg["pokemon"] = m_myPokemon.toJson();
        client->write(QJsonDocument(joinMsg).toJson(QJsonDocument::Compact) + '\n');
        client->flush();
    });

    // 接收客户端发来的精灵数据
    connect(m_host, &NetBattleHost::jsonReceived, this,
            [this](QTcpSocket* /*sender*/, const QJsonObject& obj) {
        qDebug() << "[Host] 收到 JSON 消息，type =" << obj["type"].toString();
        if (obj["type"] == "init") {
            Pokemon opponent;
            opponent.fromJson(obj["pokemon"].toObject());
            qDebug() << "[Host] 对手精灵:" << opponent.Pokemonname;

            if (m_waitDialog) {
                m_waitDialog->accept();
                m_waitDialog = nullptr;
            }
            NetFightWindow* fight = new NetFightWindow(m_player, opponent, true, m_host, nullptr, this);
            fight->setAttribute(Qt::WA_DeleteOnClose);
            fight->show();
            qDebug() << "[Host] 战斗窗口已创建";
            connect(fight, &NetFightWindow::finished, [this]() {
                if (m_host) {
                    m_host->close();
                    delete m_host;
                    m_host = nullptr;
                }
            });
        }
    });

    m_waitDialog->exec();
}

void mainwindow::joinHost()
{
    QString ip = QInputDialog::getText(this, "加入房间", "请输入主机IP地址:", QLineEdit::Normal, "127.0.0.1");
    if (ip.isEmpty()) return;

    qDebug() << "[Client] 尝试连接到主机" << ip;

    m_client = new NetBattleClient(this);
    m_client->connectToHost(ip, 8000);

    // 连接成功时触发
    connect(m_client, &NetBattleClient::connectedToHost, this, [this]() {
        qDebug() << "[Client] connectedToHost 信号触发，发送 init 消息";
        QJsonObject joinMsg;
        joinMsg["type"] = "init";
        joinMsg["pokemon"] = m_myPokemon.toJson();
        m_client->sendJson(joinMsg);
        QMessageBox::information(this, "提示", "已连接主机，等待对战开始...");
    });

    // 收到任何 JSON 消息
    connect(m_client, &NetBattleClient::jsonReceived, this, [this](const QJsonObject& obj) {
        qDebug() << "[Client] jsonReceived 信号触发，type =" << obj["type"].toString();
        if (obj["type"] == "init") {
            Pokemon opponent;
            opponent.fromJson(obj["pokemon"].toObject());
            qDebug() << "[Client] 收到对手精灵：" << opponent.Pokemonname;
            NetFightWindow* fight = new NetFightWindow(m_player, opponent, false, nullptr, m_client, this);
            fight->setAttribute(Qt::WA_DeleteOnClose);
            fight->show();
            connect(fight, &NetFightWindow::finished, [this]() {
                if (m_client) {
                    m_client->disconnectFromHost();
                    m_client->deleteLater();
                    m_client = nullptr;
                }
            });
        }
    });

    // 连接错误
    connect(m_client, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, [this](QAbstractSocket::SocketError) {
        qDebug() << "[Client] 连接错误";
        QMessageBox::critical(this, "错误", "连接失败，请检查IP和端口是否正确。");
        m_client->deleteLater();
        m_client = nullptr;
    });

    // 额外：监听 disconnected 信号
    connect(m_client, &QTcpSocket::disconnected, this, []()
    {
        qDebug() << "[Client] 连接断开";
    });
}
void mainwindow::checkEncounter()
{
    if (m_inBattle) return;
    if (!m_player || m_player->getTeamSize() == 0 || !m_wildManager) return;
    if (!m_player->hasHealthyPokemon()) return;

    // 当前精灵濒死时，自动切换成队伍里第一只健康的精灵
    if (m_player->getCurrentPokemon().hp <= 0) {
        for (int i = 0; i < m_player->team.size(); ++i)
        {
            if (m_player->team[i].hp > 0) {
                m_player->setCurrentPokemon(i);
                break;
            }
        }
    }
    if (m_player->getCurrentPokemon().hp <= 0)
        return;

    // 每移动 6 步才判定一次，避免按住方向键时遇敌过于频繁
    ++m_encounterStep;
    if (m_encounterStep % 1 != 0)
        return;

    int mapIndex = m_mapManager->currentIndex();
    QPoint center(player->x() + player->width() / 2,
                  player->y() + player->height() / 2);

    Pokemon wild;
    if (m_wildManager->roll(center, mapIndex, wild))
        startWildBattle(wild);
}
void mainwindow::startWildBattle(const Pokemon& wildPoke)
{
    if (!m_player || m_player->getTeamSize() == 0) return;

    Pokemon playerPoke = m_player->getCurrentPokemon();
    if (playerPoke.skills.isEmpty())
        playerPoke.skills.append(AllSkills::getSkill(SkillId::Tackle));

    Pokemon enemy = wildPoke;
    if (enemy.skills.isEmpty())
        enemy.skills.append(AllSkills::getSkill(SkillId::Spark));

    // 遇敌后暂停地图移动与再次判定
    timer->stop();
    dx = 0;
    dy = 0;
    m_inBattle = true;

    FightWindow* fight = new FightWindow(m_player, enemy, this);
    fight->setAttribute(Qt::WA_DeleteOnClose);

    connect(fight, &FightWindow::finished, this, [this](bool /*win*/) {
        m_inBattle = false;
        if (timer)
            timer->start();

        // 全队都倒下时自动送精灵中心恢复，避免游戏卡死
        if (!m_player->hasHealthyPokemon()) {
            m_player->healAll();
            QMessageBox::information(this, "精灵中心", "你的宝可梦都累倒了，已送精灵中心恢复完毕！");
        }
    });

    fight->show();
}

// ==========================================
// 析构函数
// ==========================================
mainwindow::~mainwindow()
{
    if (m_host) {
        m_host->close();
        delete m_host;
    }
    if (m_client) {
        m_client->disconnectFromHost();
        m_client->deleteLater();
    }
    delete m_wildManager;
    delete m_player;
    delete m_mapManager;
    delete ui;
}

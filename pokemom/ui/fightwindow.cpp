#include "fightwindow.h"
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTimer>
#include "item.h"

FightWindow::FightWindow(Player* player, const Pokemon& enemy, QWidget *parent)
    : QWidget(parent)
    , m_battle(nullptr)
    , m_timer(nullptr)
    , m_isPlayerTurn(true)
    , m_isEnd(false)
    , m_captured(false)
    , m_player(player)
{
    // 复制玩家当前精灵用于战斗
    m_playerPoke = m_player->getCurrentPokemon();
    m_enemy = enemy;

    // 立绘只在开战时加载一次，避免每 300ms 刷新重复解码图片
    if (!m_playerPoke.resBackImg.isEmpty())
        m_playerPix.load(m_playerPoke.resBackImg);
    if (m_playerPix.isNull() && !m_playerPoke.resFrontImg.isEmpty())
        m_playerPix.load(m_playerPoke.resFrontImg);
    m_enemyPix.load(m_enemy.resFrontImg);
    if (!m_playerPix.isNull())
        m_playerPixSmall = m_playerPix.scaled(140, 140, Qt::KeepAspectRatio);
    if (!m_enemyPix.isNull())
        m_enemyPixSmall = m_enemyPix.scaled(140, 140, Qt::KeepAspectRatio);

    setupUI();

    m_battle = new battlestate();
    m_battle->setupBattle(m_playerPoke, m_enemy);

    refresh();
    log(QString("%1 出现了！").arg(m_enemy.Pokemonname));

    m_timer = new QTimer(this);
    m_timer->setInterval(300);
    connect(m_timer, &QTimer::timeout, this, &FightWindow::updateStatus);
    m_timer->start();

    showMenu();
    setTurn(true);
}

FightWindow::~FightWindow()
{
    if (m_battle) delete m_battle;
    if (m_timer) delete m_timer;
}

void FightWindow::closeEvent(QCloseEvent *event)
{
    // 手动关闭战斗窗口也要通知主窗口恢复地图，避免卡死在“遇敌中”
    if (!m_isEnd) {
        m_isEnd = true;
        if (m_timer) m_timer->stop();
        syncPlayerPokemon();
        const bool win = m_battle && m_battle->state == BattleState::WIN;
        emit finished(win);
    }
    QWidget::closeEvent(event);
}

void FightWindow::setupUI()
{
    setWindowTitle("宝可梦对战");
    setFixedSize(1260, 1000);
    setStyleSheet("background: #1a2a3a;");

    QVBoxLayout* main = new QVBoxLayout(this);
    main->setContentsMargins(20, 20, 20, 20);
    main->setSpacing(10);

    // ===== 敌方 =====
    QHBoxLayout* enemyRow = new QHBoxLayout();
    enemyRow->setSpacing(15);

    m_enemyImg = new QLabel(this);
    m_enemyImg->setFixedSize(150, 150);
    m_enemyImg->setStyleSheet("background: rgba(255,255,255,0.1); border-radius: 20px;");
    m_enemyImg->setAlignment(Qt::AlignCenter);

    QVBoxLayout* enemyInfo = new QVBoxLayout();
    m_enemyName = new QLabel("敌方", this);
    m_enemyName->setStyleSheet("color: #fff; font-size: 20px; font-weight: bold;");
    m_enemyHp = new QProgressBar(this);
    m_enemyHp->setRange(0, 100);
    m_enemyHp->setFixedHeight(20);
    m_enemyHp->setStyleSheet(
        "QProgressBar { border: 2px solid #444; border-radius: 10px; background: #222; }"
        "QProgressBar::chunk { background: #44ff44; border-radius: 8px; }"
    );
    m_enemyHpText = new QLabel("HP: 0/0", this);
    m_enemyHpText->setStyleSheet("color: #aaddff;");

    enemyInfo->addWidget(m_enemyName);
    enemyInfo->addWidget(m_enemyHp);
    enemyInfo->addWidget(m_enemyHpText);

    enemyRow->addStretch();
    enemyRow->addWidget(m_enemyImg);
    enemyRow->addLayout(enemyInfo);
    enemyRow->addStretch();

    main->addLayout(enemyRow);
    main->addSpacing(20);

    // ===== 日志 =====
    m_logLabel = new QLabel(this);
    m_logLabel->setWordWrap(true);
    m_logLabel->setFixedHeight(120);
    m_logLabel->setStyleSheet("background: rgba(0,0,0,0.6); color: #FFD700; border-radius: 10px; padding: 10px; font-size: 18px;");
    m_logLabel->setAlignment(Qt::AlignCenter);
    main->addWidget(m_logLabel);

    // ===== 我方 =====
    QHBoxLayout* playerRow = new QHBoxLayout();
    playerRow->setSpacing(15);

    QVBoxLayout* playerInfo = new QVBoxLayout();
    m_playerName = new QLabel("我方", this);
    m_playerName->setStyleSheet("color: #fff; font-size: 20px; font-weight: bold;");
    m_playerHp = new QProgressBar(this);
    m_playerHp->setRange(0, 100);
    m_playerHp->setFixedHeight(20);
    m_playerHp->setStyleSheet(
        "QProgressBar { border: 2px solid #444; border-radius: 10px; background: #222; }"
        "QProgressBar::chunk { background: #44ccff; border-radius: 8px; }"
    );
    m_playerHpText = new QLabel("HP: 0/0", this);
    m_playerHpText->setStyleSheet("color: #aaddff;");

    m_playerImg = new QLabel(this);
    m_playerImg->setFixedSize(150, 150);
    m_playerImg->setStyleSheet("background: rgba(255,255,255,0.1); border-radius: 20px;");
    m_playerImg->setAlignment(Qt::AlignCenter);

    playerInfo->addWidget(m_playerName);
    playerInfo->addWidget(m_playerHp);
    playerInfo->addWidget(m_playerHpText);

    playerRow->addStretch();
    playerRow->addLayout(playerInfo);
    playerRow->addWidget(m_playerImg);
    playerRow->addStretch();

    main->addLayout(playerRow);
    main->addSpacing(15);

    // ===== 菜单 =====
    m_menuPanel = new QWidget(this);
    m_menuPanel->setStyleSheet("background: rgba(0,0,0,0.5); border-radius: 15px;");

    QHBoxLayout* menuLayout = new QHBoxLayout(m_menuPanel);
    menuLayout->setSpacing(20);

    QString btnStyle =
        "QPushButton {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #4a6fa5, stop:1 #2c4a7a);"
        "   color: white; border: 2px solid #6a9fc5; border-radius: 20px;"
        "   font-size: 24px; font-weight: bold; padding: 10px; min-width: 120px;"
        "}"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #5a7fb5, stop:1 #3c5a8a); }";

    m_btnFight = new QPushButton("⚔ 战斗", this);
    m_btnBag = new QPushButton("🎒 道具", this);   // 改为“道具”
    m_btnRun = new QPushButton("🏃 逃跑", this);
    m_btnFight->setStyleSheet(btnStyle);
    m_btnBag->setStyleSheet(btnStyle);
    m_btnRun->setStyleSheet(btnStyle);

    menuLayout->addStretch();
    menuLayout->addWidget(m_btnFight);
    menuLayout->addWidget(m_btnBag);
    menuLayout->addWidget(m_btnRun);
    menuLayout->addStretch();

    main->addWidget(m_menuPanel);

    // ===== 技能面板 =====
    m_skillPanel = new QWidget(this);
    m_skillPanel->setStyleSheet("background: rgba(0,0,0,0.5); border-radius: 15px;");
    m_skillPanel->hide();

    QGridLayout* skillLayout = new QGridLayout(m_skillPanel);
    skillLayout->setSpacing(15);

    QString skillStyle =
        "QPushButton {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #e67e22, stop:1 #d35400);"
        "   color: white; border: 2px solid #f39c12; border-radius: 20px;"
        "   font-size: 16px; font-weight: bold; padding: 12px; min-height: 78px;"
        "}"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #f39c12, stop:1 #e67e22); }"
        "QPushButton:disabled { background: #555; border-color: #777; color: #999; }";

    for (int i = 0; i < 4; i++) {
        QPushButton* btn = new QPushButton(QString("技能%1").arg(i+1), this);
        btn->setStyleSheet(skillStyle);
        m_skillBtns.append(btn);
        skillLayout->addWidget(btn, i/2, i%2);
        connect(btn, &QPushButton::clicked, this, [this, i]() { onSkill(i); });
    }

    m_btnBack = new QPushButton("🔙 返回", this);
    m_btnBack->setStyleSheet(btnStyle);
    skillLayout->addWidget(m_btnBack, 2, 0, 1, 2);
    connect(m_btnBack, &QPushButton::clicked, this, &FightWindow::onBack);

    main->addWidget(m_skillPanel);

    // 连接按钮
    connect(m_btnFight, &QPushButton::clicked, this, &FightWindow::onFight);
    connect(m_btnBag, &QPushButton::clicked, this, &FightWindow::onBag);
    connect(m_btnRun, &QPushButton::clicked, this, &FightWindow::onRun);
}

void FightWindow::refresh()
{
    // 我方
    if (!m_playerPixSmall.isNull()) {
        m_playerImg->setPixmap(m_playerPixSmall);
    } else {
        m_playerImg->setText("🐾");
        m_playerImg->setStyleSheet("color: white; font-size: 50px; background: rgba(255,255,255,0.1); border-radius: 20px;");
    }

    QString playerStatusText = damagedeal::getStatusName(static_cast<int>(m_playerPoke.status));
    QString playerTitle = QString("%1 Lv.%2").arg(m_playerPoke.Pokemonname).arg(m_playerPoke.level);
    if (m_playerPoke.status != StatusEffect::None)
        playerTitle += QString("  [%1]").arg(playerStatusText);
    m_playerName->setText(playerTitle);
    int hp = (m_playerPoke.maxhp > 0) ? (m_playerPoke.hp * 100 / m_playerPoke.maxhp) : 0;
    m_playerHp->setValue(hp);
    m_playerHpText->setText(QString("HP: %1/%2").arg(m_playerPoke.hp).arg(m_playerPoke.maxhp));

    // 敌方
    if (!m_enemyPixSmall.isNull()) {
        m_enemyImg->setPixmap(m_enemyPixSmall);
    } else {
        m_enemyImg->setText("🐾");
        m_enemyImg->setStyleSheet("color: white; font-size: 50px; background: rgba(255,255,255,0.1); border-radius: 20px;");
    }

    QString enemyStatusText = damagedeal::getStatusName(static_cast<int>(m_enemy.status));
    QString enemyTitle = QString("%1 Lv.%2").arg(m_enemy.Pokemonname).arg(m_enemy.level);
    if (m_enemy.status != StatusEffect::None)
        enemyTitle += QString("  [%1]").arg(enemyStatusText);
    m_enemyName->setText(enemyTitle);
    hp = (m_enemy.maxhp > 0) ? (m_enemy.hp * 100 / m_enemy.maxhp) : 0;
    m_enemyHp->setValue(hp);
    m_enemyHpText->setText(QString("HP: %1/%2").arg(m_enemy.hp).arg(m_enemy.maxhp));
}

void FightWindow::showMenu()
{
    m_menuPanel->show();
    m_skillPanel->hide();
    setTurn(m_isPlayerTurn);
}

void FightWindow::showSkills()
{
    m_menuPanel->hide();
    m_skillPanel->show();

    int count = m_playerPoke.skills.size();
    for (int i = 0; i < m_skillBtns.size(); i++) {
        if (i < count) {
            const Skill& skill = m_playerPoke.skills[i];

            QString typeName;
            switch (skill.skilltype) {
                case 1: typeName = "火"; break;
                case 2: typeName = "水"; break;
                case 3: typeName = "草"; break;
                default: typeName = "普通"; break;
            }

            QString powerText = (skill.power > 0)
                ? QString("威力 %1").arg(skill.power)
                : QString("变化技");

            m_skillBtns[i]->setText(QString("%1\n%2 · %3")
                                        .arg(skill.Skillname)
                                        .arg(typeName)
                                        .arg(powerText));
            m_skillBtns[i]->setToolTip(QString("命中率 %1%").arg(skill.accuracy));
            m_skillBtns[i]->setEnabled(m_isPlayerTurn && !m_isEnd);
            m_skillBtns[i]->show();
        } else {
            m_skillBtns[i]->hide();
        }
    }
}

void FightWindow::setTurn(bool turn)
{
    m_isPlayerTurn = turn;
    m_btnFight->setEnabled(turn && !m_isEnd);
    m_btnBag->setEnabled(turn && !m_isEnd);
    m_btnRun->setEnabled(turn && !m_isEnd);
    for (auto btn : m_skillBtns) {
        btn->setEnabled(turn && !m_isEnd);
    }
}

void FightWindow::log(const QString& msg)
{
    m_logLabel->setText(msg);
}

void FightWindow::updateStatus()
{
    if (!m_battle) return;
    m_playerPoke = m_battle->playerPoke;
    m_enemy = m_battle->enemyPoke;
    refresh();

    if (m_battle->state == BattleState::WIN || m_battle->state == BattleState::LOSE) {
        onEnd();
    }
}

void FightWindow::onFight()
{
    if (!m_isPlayerTurn || m_isEnd) return;
    showSkills();
}

void FightWindow::onBag()
{
    if (!m_isPlayerTurn || m_isEnd) return;
    showItemMenu();
}

void FightWindow::onRun()
{
    if (!m_isPlayerTurn || m_isEnd) return;
    syncPlayerPokemon();
    log("你逃跑了！");
    m_isEnd = true;
    m_timer->stop();
    QTimer::singleShot(1000, this, [this]() {
        emit finished(false);
        close();
    });
}

void FightWindow::onSkill(int index)
{
    if (!m_isPlayerTurn || m_isEnd || !m_battle) return;

    setTurn(false);
    m_battle->playerUseSkill(index);
    log(m_battle->lastLog);

    updateStatus();

    if (!m_isEnd) {
        QTimer::singleShot(900, this, &FightWindow::onEnemyAction);
    }
}

void FightWindow::onEnemyAction()
{
    if (m_isEnd || !m_battle) return;

    DamageResult enemyRes;
    m_battle->enemyUseSkill(&enemyRes);
    log(m_battle->lastLog);

    updateStatus();

    if (!m_isEnd) {
        QTimer::singleShot(700, this, [this]() {
            setTurn(true);
            showMenu();
            log("选择你的行动！");
        });
    }
}

void FightWindow::onEnd()
{
    if (m_isEnd) return;
    m_isEnd = true;
    m_timer->stop();

    bool win = (m_battle->state == BattleState::WIN);

    // ----- 战斗结果处理 -----
    if (win) {
        if (m_captured) {
            log("捕捉成功！新伙伴已加入队伍/仓库");
            m_logLabel->setStyleSheet(
                "background: rgba(0,100,0,0.7); color: #FFD700; "
                "border-radius: 10px; padding: 10px; font-size: 20px;"
            );
        } else {
            // 1. 计算经验值（基于敌方等级）
            int expGain = qMax(10, m_enemy.level * 6 + 8);

            // 2. 让出战精灵获得经验（会自动升级）
            m_playerPoke.gainExp(expGain);

            // 3. 显示胜利信息
            log("胜利！获得 " + QString::number(expGain) + " 经验");
        m_logLabel->setStyleSheet(
            "background: rgba(0,100,0,0.7); color: #FFD700; "
            "border-radius: 10px; padding: 10px; font-size: 20px;"
        );
        }
    } else {

        log("失败...");
        m_logLabel->setStyleSheet(
            "background: rgba(100,0,0,0.7); color: white; "
            "border-radius: 10px; padding: 10px; font-size: 20px;"
        );
    }

    // 无论胜负/逃跑，都把出战精灵的当前状态同步回玩家队伍
    syncPlayerPokemon();

    // 延迟关闭
    QTimer::singleShot(2000, this, [this, win]() {
        emit finished(win);
        close();
    });
}
void FightWindow::syncPlayerPokemon()
{
    if (!m_player ||
        m_player->currentPokemonIndex < 0 ||
        m_player->currentPokemonIndex >= m_player->team.size()) {
        return;
    }
    m_player->team[m_player->currentPokemonIndex] = m_playerPoke;
}
void FightWindow::onBack()
{
    showMenu();
}

// ===== 新增道具相关函数 =====
void FightWindow::showItemMenu()
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("选择道具");
    dialog->setModal(true);
    dialog->resize(300, 400);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QListWidget* list = new QListWidget(dialog);
    layout->addWidget(list);

    QList<int> itemIds = m_player->bag.getAllItemIds();
    for (int id : itemIds) {
        int count = m_player->bag.getItemCount(id);
        if (count <= 0) continue;
        QString name;
        if (id == ITEM_POTION) name = "药水";
        else if (id == ITEM_POKEBALL) name = "精灵球";
        else name = "未知";
        QListWidgetItem* item = new QListWidgetItem(QString("%1 x%2").arg(name).arg(count), list);
        item->setData(Qt::UserRole, id);
        list->addItem(item);
    }

    if (list->count() == 0) {
        QListWidgetItem* empty = new QListWidgetItem("背包为空", list);
        empty->setTextAlignment(Qt::AlignCenter);
        list->addItem(empty);
    }

    connect(list, &QListWidget::itemDoubleClicked, this, [this, dialog, list](QListWidgetItem* item)
    {
        int id = item->data(Qt::UserRole).toInt();
        if (id == 0) return;
        dialog->accept();
        onUseItem(id);
    });

    QPushButton* cancelBtn = new QPushButton("取消", dialog);
    connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::reject);
    layout->addWidget(cancelBtn);

    dialog->exec();
    delete dialog;
}

void FightWindow::onUseItem(int itemId)
{
    if (itemId == ITEM_POTION && m_playerPoke.hp >= m_playerPoke.maxhp) {
        log("HP 已满，无需使用药水");
        return;
    }
    if (!m_player->bag.consumeItem(itemId)) {
        log("道具不足！");
        return;
    }

    if (itemId == ITEM_POTION) {
        int heal = qMax(20, m_playerPoke.maxhp / 3);
        int oldHp = m_playerPoke.hp;
        m_playerPoke.hp = qMin(m_playerPoke.hp + heal, m_playerPoke.maxhp);
        int actualHeal = m_playerPoke.hp - oldHp;
        log(QString("使用了药水，恢复了 %1 HP").arg(actualHeal));
        refresh();
        setTurn(false);
        QTimer::singleShot(500, this, &FightWindow::onEnemyAction);
    }
    else if (itemId == ITEM_POKEBALL) {
        tryCapture();
    }
}

void FightWindow::tryCapture()
{
    // 血量越低越容易捕获：满血 25%，残血接近 90%
    double hpRatio = m_enemy.maxhp > 0 ? (double)m_enemy.hp / m_enemy.maxhp : 1.0;
    double rate = 0.25 + 0.65 * (1.0 - hpRatio);
    float rand = QRandomGenerator::global()->generateDouble();
    bool success = rand < rate;

    if (success) {
        m_captured = true;
        log("捕捉成功！");
        Pokemon captured = m_enemy;
        captured.hp = captured.maxhp;
        captured.status = StatusEffect::None;
        captured.statusTurns = 0;
        m_player->addPokemon(captured);
        emit captureSuccess(captured);

        m_battle->state = BattleState::WIN;
        onEnd();
    } else {
        log("捕捉失败...");
        setTurn(false);
        QTimer::singleShot(500, this, &FightWindow::onEnemyAction);
    }
}

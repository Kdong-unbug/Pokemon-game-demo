#include "pokemoninfo.h"

BagWindow::BagWindow(Player* player, QWidget *parent)
    : QWidget(parent)
    , m_player(player)
    , m_selectedIndex(-1)
{
    setupUI();
    refreshList();
}

BagWindow::~BagWindow() = default;

void BagWindow::setupUI()
{
    setWindowTitle("宝可梦背包");
    setFixedSize(1300, 950);
    setStyleSheet("background: #1a2a3a;");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(30);

    // ===== 左侧：队伍列表 =====
    QWidget* leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(500);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(20);

    QLabel* title = new QLabel("✦ 我的队伍 ✦", this);
    title->setStyleSheet("color: #FFD700; font-size: 32px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(title);

    QLabel* countLabel = new QLabel(QString("共 %1 只宝可梦").arg(m_player ? m_player->getTeamSize() : 0), this);
    countLabel->setStyleSheet("color: #aaddff; font-size: 18px;");
    countLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(countLabel);

    m_listWidget = new QListWidget(this);
    m_listWidget->setStyleSheet(
        "QListWidget {"
        "   background: rgba(255,255,255,0.05);"
        "   border: 2px solid #4a6fa5;"
        "   border-radius: 15px;"
        "   padding: 15px;"
        "   font-size: 20px;"
        "}"
        "QListWidget::item {"
        "   color: white;"
        "   padding: 18px;"
        "   border-bottom: 1px solid rgba(255,255,255,0.08);"
        "}"
        "QListWidget::item:selected {"
        "   background: rgba(74,111,165,0.5);"
        "   border-radius: 10px;"
        "}"
        "QListWidget::item:hover {"
        "   background: rgba(255,255,255,0.1);"
        "   border-radius: 10px;"
        "}"
    );
    m_listWidget->setIconSize(QSize(70, 70));
    connect(m_listWidget, &QListWidget::itemClicked, this, &BagWindow::onPokemonSelected);
    leftLayout->addWidget(m_listWidget);

    QPushButton* closeBtn = new QPushButton("✕ 关闭", this);
    closeBtn->setFixedHeight(60);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #e74c3c, stop:1 #c0392b);"
        "   color: white; border: none; border-radius: 20px;"
        "   font-size: 22px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #ff6b6b, stop:1 #e74c3c);"
        "}"
    );
    connect(closeBtn, &QPushButton::clicked, this, &BagWindow::onClose);
    leftLayout->addWidget(closeBtn);

    mainLayout->addWidget(leftPanel);

    // ===== 右侧：详情面板 =====
    m_detailPanel = new QWidget(this);
    m_detailPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_detailPanel->setMinimumWidth(650);
    // Qt5 QWidget不识别padding，删掉padding:30px
    m_detailPanel->setStyleSheet("background: rgba(40, 50, 60, 0.95); border-radius: 15px;");
    QVBoxLayout* detailLayout = new QVBoxLayout(m_detailPanel);
    detailLayout->setContentsMargins(30, 30, 30, 30);
    detailLayout->setSpacing(25);

    // 精灵图片
    m_detailImg = new QLabel(this);
    m_detailImg->setFixedSize(300, 300);
    m_detailImg->setStyleSheet("background: rgba(255,255,255,0.05); border-radius: 20px;");
    m_detailImg->setAlignment(Qt::AlignCenter);
    detailLayout->addWidget(m_detailImg, 0, Qt::AlignCenter);

    // ========= 仅此处增加两行属性，Qt5完全可用 =========
    m_detailName = new QLabel("选择一只宝可梦查看详情", this);
    m_detailName->setStyleSheet("color: #FFD700; font-size: 34px; font-weight: bold;");
    m_detailName->setAlignment(Qt::AlignCenter);
    m_detailName->setWordWrap(true);
    m_detailName->setMaximumWidth(580);
    m_detailName->setMinimumHeight(50);
    detailLayout->addWidget(m_detailName);

    // ===== 两列网格布局 =====
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);
    gridLayout->setHorizontalSpacing(40);
    gridLayout->setVerticalSpacing(12);

    QString labelStyle = "color: #88aacc; font-size: 22px; font-weight: bold;";
    QString valueStyle = "color: #ffffff; font-size: 22px;";

    // 第1行：等级
    QLabel* lvLabel = new QLabel("🎯 等级:", this);
    lvLabel->setStyleSheet(labelStyle);
    lvLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailLevel = new QLabel("-", this);
    m_detailLevel->setStyleSheet(valueStyle);
    m_detailLevel->setMinimumHeight(40);

    // 第2行：经验
    QLabel* expLabel = new QLabel("⭐ 经验:", this);
    expLabel->setStyleSheet(labelStyle);
    expLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailExp = new QLabel("-", this);
    m_detailExp->setStyleSheet(valueStyle);
    m_detailExp->setMinimumHeight(40);

    // 第3行：HP
    QLabel* hpLabel = new QLabel("❤️ HP:", this);
    hpLabel->setStyleSheet(labelStyle);
    hpLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailHp = new QLabel("-", this);
    m_detailHp->setStyleSheet("color: #88ff88; font-size: 22px;");
    m_detailHp->setMinimumHeight(40);

    // 第4行：最大HP
    QLabel* maxHpLabel = new QLabel("💚 最大HP:", this);
    maxHpLabel->setStyleSheet(labelStyle);
    maxHpLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailMaxHp = new QLabel("-", this);
    m_detailMaxHp->setStyleSheet("color: #88ff88; font-size: 22px;");
    m_detailMaxHp->setMinimumHeight(40);

    // 第5行：攻击
    QLabel* atkLabel = new QLabel("⚔️ 攻击:", this);
    atkLabel->setStyleSheet(labelStyle);
    atkLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailAtk = new QLabel("-", this);
    m_detailAtk->setStyleSheet("color: #ff8844; font-size: 22px;");
    m_detailAtk->setMinimumHeight(40);

    // 第6行：防御
    QLabel* defLabel = new QLabel("🛡️ 防御:", this);
    defLabel->setStyleSheet(labelStyle);
    defLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailDef = new QLabel("-", this);
    m_detailDef->setStyleSheet("color: #44aaff; font-size: 22px;");
    m_detailDef->setMinimumHeight(40);

    // 第7行：属性
    QLabel* typeLabel = new QLabel("🔥 属性:", this);
    typeLabel->setStyleSheet(labelStyle);
    typeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailType = new QLabel("-", this);
    m_detailType->setStyleSheet("color: #ffdd44; font-size: 22px;");
    m_detailType->setMinimumHeight(40);

    // 第8行：状态
    QLabel* statusLabel = new QLabel("📊 状态:", this);
    statusLabel->setStyleSheet(labelStyle);
    statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailStatus = new QLabel("正常", this);
    m_detailStatus->setStyleSheet("color: #88ff88; font-size: 22px;");
    m_detailStatus->setMinimumHeight(40);

    // 第9行：技能（占两列）
    QLabel* skillLabel = new QLabel("💫 技能:", this);
    skillLabel->setStyleSheet(labelStyle);
    skillLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_detailSkill = new QLabel("-", this);
    m_detailSkill->setStyleSheet("color: #aa88ff; font-size: 20px;");
    m_detailSkill->setWordWrap(true);
    m_detailSkill->setMinimumHeight(60);

    // 添加到网格（两列）
    gridLayout->addWidget(lvLabel, 0, 0);
    gridLayout->addWidget(m_detailLevel, 0, 1);
    gridLayout->addWidget(expLabel, 1, 0);
    gridLayout->addWidget(m_detailExp, 1, 1);
    gridLayout->addWidget(hpLabel, 2, 0);
    gridLayout->addWidget(m_detailHp, 2, 1);
    gridLayout->addWidget(maxHpLabel, 3, 0);
    gridLayout->addWidget(m_detailMaxHp, 3, 1);
    gridLayout->addWidget(atkLabel, 4, 0);
    gridLayout->addWidget(m_detailAtk, 4, 1);
    gridLayout->addWidget(defLabel, 5, 0);
    gridLayout->addWidget(m_detailDef, 5, 1);
    gridLayout->addWidget(typeLabel, 6, 0);
    gridLayout->addWidget(m_detailType, 6, 1);
    gridLayout->addWidget(statusLabel, 7, 0);
    gridLayout->addWidget(m_detailStatus, 7, 1);
    gridLayout->addWidget(skillLabel, 8, 0);
    gridLayout->addWidget(m_detailSkill, 8, 1);

    detailLayout->addLayout(gridLayout);

    // 导航按钮
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->setSpacing(30);

    m_prevBtn = new QPushButton("◀ 上一只", this);
    m_nextBtn = new QPushButton("下一只 ▶", this);
    m_prevBtn->setFixedHeight(60);
    m_nextBtn->setFixedHeight(60);
    m_prevBtn->setEnabled(false);
    m_nextBtn->setEnabled(false);

    QString navStyle =
        "QPushButton {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #4a6fa5, stop:1 #2c4a7a);"
        "   color: white; border: 2px solid #6a9fc5; border-radius: 20px;"
        "   font-size: 20px;"
        "   font-weight: bold; padding: 15px 30px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #5a7fb5, stop:1 #3c5a8a);"
        "}"
        "QPushButton:disabled {"
        "   background: #555; border-color: #777; color: #999;"
        "}";

    m_prevBtn->setStyleSheet(navStyle);
    m_nextBtn->setStyleSheet(navStyle);

    m_activeBtn = new QPushButton("设为出战", this);
    m_activeBtn->setFixedHeight(60);
    m_activeBtn->setStyleSheet(navStyle);
    connect(m_activeBtn, &QPushButton::clicked, this, &BagWindow::onSetActivePokemon);

    connect(m_prevBtn, &QPushButton::clicked, this, &BagWindow::onPrevPokemon);
    connect(m_nextBtn, &QPushButton::clicked, this, &BagWindow::onNextPokemon);

    navLayout->addStretch();
    navLayout->addWidget(m_prevBtn);
    navLayout->addWidget(m_activeBtn);
    navLayout->addWidget(m_nextBtn);
    navLayout->addStretch();

    detailLayout->addLayout(navLayout);

    mainLayout->addWidget(m_detailPanel);

    // 默认显示
    m_detailName->setText("👆 选择一只宝可梦");
    m_detailImg->setText("🐾");
    m_detailImg->setStyleSheet("font-size: 100px; background: rgba(255,255,255,0.05); border-radius: 20px;");
}

void BagWindow::refreshList()
{
    m_listWidget->clear();

    if (!m_player) {
        QListWidgetItem* empty = new QListWidgetItem("❌ 玩家数据为空", m_listWidget);
        empty->setTextAlignment(Qt::AlignCenter);
        m_listWidget->addItem(empty);
        return;
    }

    int teamSize = m_player->getTeamSize();

    if (teamSize == 0) {
        QListWidgetItem* empty = new QListWidgetItem("⚠️ 没有宝可梦", m_listWidget);
        empty->setTextAlignment(Qt::AlignCenter);
        m_listWidget->addItem(empty);
        m_detailName->setText("⚠️ 没有宝可梦");
        m_detailImg->setText("😢");
        return;
    }

    for (int i = 0; i < teamSize; i++) {
        Pokemon& poke = m_player->team[i];

        QString hpStatus = (poke.hp <= 0) ? "💀" :
                          (poke.hp < poke.maxhp / 3) ? "⚠️" : "✅";

        QString activeMark = (i == m_player->currentPokemonIndex) ? " ★" : "";
        QString text = QString("%1%2  %3  Lv.%4  HP:%5/%6")
                       .arg(poke.Pokemonname)
                       .arg(activeMark)
                       .arg(hpStatus)
                       .arg(poke.level)
                       .arg(poke.hp)
                       .arg(poke.maxhp);

        QListWidgetItem* item = new QListWidgetItem(text, m_listWidget);

        if (!poke.resFrontImg.isEmpty()) {
            QPixmap p;
            if (p.load(poke.resFrontImg)) {
                if (!p.isNull()) {
                    item->setIcon(QIcon(p.scaled(70, 70, Qt::KeepAspectRatio)));
                }
            }
        }

        if (poke.hp <= 0) {
            item->setForeground(QColor(255, 80, 80));
        } else if (poke.hp < poke.maxhp / 3) {
            item->setForeground(QColor(255, 180, 50));
        }

        item->setData(Qt::UserRole, i);
        m_listWidget->addItem(item);
    }

    // 默认选中第一个
    if (teamSize > 0) {
        m_selectedIndex = 0;
        showPokemonDetail(0);
        m_listWidget->setCurrentRow(0);
        m_prevBtn->setEnabled(false);
        m_nextBtn->setEnabled(teamSize > 1);
    }
}

void BagWindow::onPokemonSelected(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    int index = item->data(Qt::UserRole).toInt();

    if (index >= 0 && index < m_player->getTeamSize()) {
        m_selectedIndex = index;
        showPokemonDetail(index);
        m_prevBtn->setEnabled(index > 0);
        m_nextBtn->setEnabled(index < m_player->getTeamSize() - 1);
    }
}

void BagWindow::showPokemonDetail(int index)
{
    if (index < 0 || index >= m_player->getTeamSize()) {
        return;
    }

    Pokemon& poke = m_player->team[index];

    // 图片
    QPixmap p;
    if (!poke.resFrontImg.isEmpty() && p.load(poke.resFrontImg) && !p.isNull()) {
        m_detailImg->setPixmap(p.scaled(300, 300, Qt::KeepAspectRatio));
        m_detailImg->setStyleSheet("background: rgba(255,255,255,0.05); border-radius: 20px;");
    } else {
        m_detailImg->setText("🐾");
        m_detailImg->setStyleSheet("font-size: 100px; background: rgba(255,255,255,0.05); border-radius: 20px;");
    }

    // ========= 仅改这里：出战标记换行，Qt5支持\n =========
    QString nameText = poke.Pokemonname;
    if (index == m_player->currentPokemonIndex) {
        nameText += "\n⭐ (出战)";
    }
    m_detailName->setText(nameText);

    // 各项数值
    m_detailLevel->setText(QString::number(poke.level));
    m_detailExp->setText(QString::number(poke.exp) + " / " + QString::number(poke.maxexp));
    m_detailHp->setText(QString::number(poke.hp));
    m_detailMaxHp->setText(QString::number(poke.maxhp));
    m_detailAtk->setText(QString::number(poke.attack));
    m_detailDef->setText(QString::number(poke.defense));

    // 属性
    QString typeName;
    switch(poke.elemType) {
        case 0: typeName = "❓ 普通"; break;
        case 1: typeName = "🔥 火系"; break;
        case 2: typeName = "💧 水系"; break;
        case 3: typeName = "🌿 草系"; break;
        default: typeName = "❓ 未知";
    }
    m_detailType->setText(typeName);

    // 状态
    if (poke.hp <= 0) {
        m_detailStatus->setText("💀 濒死");
        m_detailStatus->setStyleSheet("color: #ff4444; font-size: 22px; font-weight: bold;");
    } else if (poke.hp < poke.maxhp / 3) {
        m_detailStatus->setText("⚠️ 重伤");
        m_detailStatus->setStyleSheet("color: #ffaa44; font-size: 22px; font-weight: bold;");
    } else if (poke.hp < poke.maxhp * 2 / 3) {
        m_detailStatus->setText("💪 良好");
        m_detailStatus->setStyleSheet("color: #88ddff; font-size: 22px; font-weight: bold;");
    } else {
        m_detailStatus->setText("💚 健康");
        m_detailStatus->setStyleSheet("color: #88ff88; font-size: 22px; font-weight: bold;");
    }

    // 出战操作
    const bool isActive = (index == m_player->currentPokemonIndex);
    m_activeBtn->setText(isActive ? "当前出战" : "设为出战");
    m_activeBtn->setEnabled(!isActive && poke.hp > 0);

    // 技能
    QString skillText;
    if (poke.skills.isEmpty()) {
        skillText = "❌ 无技能";
    } else {
        for (int i = 0; i < poke.skills.size(); i++) {
            skillText += poke.skills[i].Skillname + " (威力:" + QString::number(poke.skills[i].power) + ")";
            if (i < poke.skills.size() - 1) skillText += "  |  ";
        }
    }
    m_detailSkill->setText(skillText);

    // 导航按钮
    m_prevBtn->setEnabled(index > 0);
    m_nextBtn->setEnabled(index < m_player->getTeamSize() - 1);
    m_listWidget->setCurrentRow(index);
}

void BagWindow::onPrevPokemon()
{
    if (m_selectedIndex > 0) {
        m_selectedIndex--;
        showPokemonDetail(m_selectedIndex);
        m_listWidget->setCurrentRow(m_selectedIndex);
        m_prevBtn->setEnabled(m_selectedIndex > 0);
        m_nextBtn->setEnabled(m_selectedIndex < m_player->getTeamSize() - 1);
    }
}

void BagWindow::onNextPokemon()
{
    if (m_selectedIndex < m_player->getTeamSize() - 1) {
        m_selectedIndex++;
        showPokemonDetail(m_selectedIndex);
        m_listWidget->setCurrentRow(m_selectedIndex);
        m_prevBtn->setEnabled(m_selectedIndex > 0);
        m_nextBtn->setEnabled(m_selectedIndex < m_player->getTeamSize() - 1);
    }
}

void BagWindow::onSetActivePokemon()
{
    if (!m_player || m_selectedIndex < 0 || m_selectedIndex >= m_player->getTeamSize()) {
        return;
    }

    Pokemon& selected = m_player->team[m_selectedIndex];
    if (selected.hp <= 0) {
        return;
    }

    m_player->setCurrentPokemon(m_selectedIndex);
    refreshList();

    m_selectedIndex = m_selectedIndex;
    showPokemonDetail(m_selectedIndex);
    m_listWidget->setCurrentRow(m_selectedIndex);
}

void BagWindow::onClose()
{
    close();
}

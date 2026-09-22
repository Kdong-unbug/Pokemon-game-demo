#include "shopwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "item.h"

ShopWindow::ShopWindow(Player* player, QWidget *parent)
    : QDialog(parent), m_player(player)
{
    setWindowTitle("商店");
    setModal(true);
    resize(500, 400);
    loadShopItems();
    setupUI();
    refreshUI();
}

void ShopWindow::loadShopItems()
{
    // 初始化商品列表（可后续从配置文件读取）
    m_shopItems = {
        {ITEM_POTION, "药水", 10},
        {ITEM_POKEBALL, "精灵球", 50}
        // 可添加更多商品
    };
}

void ShopWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 显示金币
    m_moneyLabel = new QLabel(this);
    m_moneyLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFD700;");

    // 购买列表
    m_buyList = new QListWidget(this);
    m_buyList->setFixedHeight(150);
    for (const auto& item : m_shopItems) {
        QString text = QString("%1   价格：%2 金币").arg(item.name).arg(item.price);
        QListWidgetItem* listItem = new QListWidgetItem(text, m_buyList);
        listItem->setData(Qt::UserRole, item.id);
        listItem->setData(Qt::UserRole + 1, item.price);
        m_buyList->addItem(listItem);
    }

    // 购买按钮
    m_buyBtn = new QPushButton("购买", this);
    connect(m_buyBtn, &QPushButton::clicked, this, &ShopWindow::onBuyClicked);

    // 出售列表（预留）
    m_sellList = new QListWidget(this);
    m_sellList->setFixedHeight(150);
    m_sellList->setEnabled(false); // 暂不实现出售，可启用

    m_sellBtn = new QPushButton("出售", this);
    m_sellBtn->setEnabled(false);  // 暂不实现
    connect(m_sellBtn, &QPushButton::clicked, this, &ShopWindow::onSellClicked);

    // 关闭按钮
    m_closeBtn = new QPushButton("离开商店", this);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    // 布局
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_buyBtn);
    btnLayout->addWidget(m_sellBtn);
    btnLayout->addStretch();

    mainLayout->addWidget(m_moneyLabel);
    mainLayout->addWidget(new QLabel("购买列表：", this));
    mainLayout->addWidget(m_buyList);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(new QLabel("出售列表（开发中）：", this));
    mainLayout->addWidget(m_sellList);
    mainLayout->addWidget(m_closeBtn);
}

void ShopWindow::refreshUI()
{
    m_moneyLabel->setText(QString("💰 金币：%1").arg(m_player->money));

    // 更新出售列表（显示背包中的物品）
    m_sellList->clear();
    QList<int> itemIds = m_player->bag.getAllItemIds();
    for (int id : itemIds) {
        int count = m_player->bag.getItemCount(id);
        if (count <= 0) continue;
        QString name;
        if (id == ITEM_POTION) name = "药水";
        else if (id == ITEM_POKEBALL) name = "精灵球";
        else name = "未知";
        QString text = QString("%1 x%2  卖出价：%3 金币")
                          .arg(name)
                          .arg(count)
                          .arg(getSellPrice(id));
        QListWidgetItem* item = new QListWidgetItem(text, m_sellList);
        item->setData(Qt::UserRole, id);
        m_sellList->addItem(item);
    }
    if (m_sellList->count() == 0) {
        new QListWidgetItem("背包为空", m_sellList);
    }
}

int ShopWindow::getSellPrice(int itemId)
{
    // 出售价为购买价的一半（向下取整）
    for (const auto& shopItem : m_shopItems) {
        if (shopItem.id == itemId) {
            return shopItem.price / 2;
        }
    }
    return 0;
}

void ShopWindow::onBuyClicked()
{
    QListWidgetItem* cur = m_buyList->currentItem();
    if (!cur) {
        QMessageBox::warning(this, "提示", "请先选择要购买的商品");
        return;
    }
    int itemId = cur->data(Qt::UserRole).toInt();
    int price = cur->data(Qt::UserRole + 1).toInt();

    if (m_player->money < price) {
        QMessageBox::warning(this, "提示", "金币不足！");
        return;
    }

    m_player->spendMoney(price);
    m_player->bag.addItem(itemId, 1);
    refreshUI();
    QMessageBox::information(this, "购买成功",
        QString("成功购买了 %1！剩余金币：%2")
        .arg(cur->text().split("   ").first())
        .arg(m_player->money));
}

void ShopWindow::onSellClicked()
{
    // 出售功能待实现
    QMessageBox::information(this, "提示", "出售功能开发中");
}

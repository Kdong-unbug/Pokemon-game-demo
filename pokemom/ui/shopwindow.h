#ifndef SHOPWINDOW_H
#define SHOPWINDOW_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include "player.h"

class ShopWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ShopWindow(Player* player, QWidget *parent = nullptr);

private slots:
    void onBuyClicked();
    void onSellClicked();   // 预留出售功能
    void refreshUI();
private:
    int getSellPrice(int itemId);
private:
    void setupUI();
    void loadShopItems();

    Player* m_player;

    // UI控件
    QListWidget* m_buyList;
    QListWidget* m_sellList;
    QLabel* m_moneyLabel;
    QPushButton* m_buyBtn;
    QPushButton* m_sellBtn;
    QPushButton* m_closeBtn;

    // 商品数据结构
    struct ShopItem {
        int id;
        QString name;
        int price;
    };
    QList<ShopItem> m_shopItems;
};

#endif // SHOPWINDOW_H

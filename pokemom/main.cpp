#include "mainwindow.h"
#include <QApplication>
#include "allskills.h"
#include "allpokemon.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

int main(int argc, char *argv[])
{
    // 忽略 PNG 警告
    QLoggingCategory::setFilterRules("qt.gui.png.warning=false");

    QApplication a(argc, argv);

    // 初始化全局数据（只需一次）
    AllSkills::init();
    allpokemon::initAllTemplate();

    // ============================================================
    // 开始菜单（无半透明面板，大按钮）
    // ============================================================
    QWidget startMenu;
    startMenu.setWindowTitle("宝可梦 开始菜单");
    startMenu.setFixedSize(1260, 1000);
    // 背景图（请确保图片路径正确）
    startMenu.setStyleSheet(
        "background-image: url(:/new/prefix1/pic/map0.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "background-size: cover;"
    );

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&startMenu);
    mainLayout->setContentsMargins(80, 80, 80, 80);
    mainLayout->setSpacing(40);

    // ---- 标题 ----
    QLabel *title = new QLabel("✨ 宝可梦 训练家 ✨", &startMenu);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: #FFD700;"
        "font-size: 64px;"
        "font-weight: bold;"
        "font-family: 'Microsoft YaHei';"
        "padding: 20px;"
        "background: rgba(0,0,0,0.3);"  // 轻微文字阴影，提高可读性
        "border-radius: 15px;"
    );
    mainLayout->addWidget(title);

    // ---- 副标题 ----
    QLabel *subTitle = new QLabel("—— 踏上冒险之旅 ——", &startMenu);
    subTitle->setAlignment(Qt::AlignCenter);
    subTitle->setStyleSheet(
        "color: #88aadd;"
        "font-size: 28px;"
        "background: rgb(0,0,9);"
        "border-radius: 10px;"
        "padding: 10px;"
    );
    mainLayout->addWidget(subTitle);

    // 弹性空间，将按钮推到底部
    mainLayout->addStretch();

    // ---- 按钮容器（水平居中） ----
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(60);

    QPushButton *startBtn = new QPushButton("- 开始游戏 -", &startMenu);
    QPushButton *exitBtn = new QPushButton("- 退出游戏 -", &startMenu);

    // 按钮样式：大按钮
    QString btnStyle =
        "QPushButton {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "       stop:0 #4a6fa5, stop:1 #2c4a7a);"
        "   color: white;"
        "   border: 4px solid #FFD700;"
        "   border-radius: 30px;"
        "   font-size: 40px;"
        "   font-weight: bold;"
        "   padding: 25px 80px;"
        "   min-width: 320px;"
        "   min-height: 100px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "       stop:0 #5a7fb5, stop:1 #3c5a8a);"
        "   border-color: #FFA500;"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "       stop:0 #2c4a7a, stop:1 #1a2a4a);"
        "}";

    startBtn->setStyleSheet(btnStyle);
    exitBtn->setStyleSheet(btnStyle);

    btnLayout->addStretch();
    btnLayout->addWidget(startBtn);
    btnLayout->addWidget(exitBtn);
    btnLayout->addStretch();

    mainLayout->addLayout(btnLayout);

    // ---- 信号连接 ----
    QObject::connect(startBtn, &QPushButton::clicked, [&]() {
        startMenu.close();                  // 关闭开始菜单
        mainwindow *w = new mainwindow();   // 在主堆上创建
        w->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动销毁
        w->show();
    });

    QObject::connect(exitBtn, &QPushButton::clicked, &a, &QApplication::quit);

    // ---- 显示菜单 ----
    startMenu.show();

    // ---- 进入事件循环 ----
    int result = a.exec();

    // ---- 程序退出前清理 ----
    qDebug() << "清理资源...";
    allpokemon::clearTemplates();
    AllSkills::clear();
    qDebug() << "资源清理完成";

    return result;
}

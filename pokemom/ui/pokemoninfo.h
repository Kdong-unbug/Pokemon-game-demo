#ifndef POKEMONINFO_H
#define POKEMONINFO_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include "pokemon.h"
#include "player.h"

class BagWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BagWindow(Player* player, QWidget *parent = nullptr);
    ~BagWindow();

private slots:
    void onPokemonSelected(QListWidgetItem* item);
    void onClose();
    void onPrevPokemon();
    void onNextPokemon();
    void onSetActivePokemon();

private:
    void setupUI();
    void refreshList();
    void showPokemonDetail(int index);

    Player* m_player;
    QListWidget* m_listWidget;
    QWidget* m_detailPanel;
    int m_selectedIndex;

    // 详情控件
    QLabel* m_detailImg;
    QLabel* m_detailName;
    QLabel* m_detailLevel;
    QLabel* m_detailExp;
    QLabel* m_detailHp;
    QLabel* m_detailMaxHp;
    QLabel* m_detailAtk;
    QLabel* m_detailDef;
    QLabel* m_detailType;
    QLabel* m_detailStatus;
    QLabel* m_detailSkill;

    QPushButton* m_prevBtn;
    QPushButton* m_nextBtn;
    QPushButton* m_activeBtn;
};

#endif // POKEMONINFO_H

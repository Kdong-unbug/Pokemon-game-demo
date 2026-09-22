QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = pokemon.ico
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    common/gamedefine.cpp \
    data/allskills.cpp \
    data/allpokemon.cpp \
    data/bag.cpp \
    data/item.cpp \
    data/player.cpp \
    data/pokemon.cpp \
    data/skill.cpp \
    battle/battlestate.cpp \
    battle/damagedeal.cpp \
    battle/wildencounter.cpp \
    map/mapmanager.cpp \
    network/netbattleclient.cpp \
    network/netbattlehost.cpp \
    network/netfightwindow.cpp \
    ui/fightwindow.cpp \
    ui/mainwindow.cpp \
    ui/npcdialog.cpp \
    ui/pokemoninfo.cpp \
    ui/shopwindow.cpp

HEADERS += \
    common/gamedefine.h \
    data/allskills.h \
    data/allpokemon.h \
    data/bag.h \
    data/item.h \
    data/player.h \
    data/pokemon.h \
    data/skill.h \
    battle/battlestate.h \
    battle/damagedeal.h \
    battle/wildencounter.h \
    map/mapmanager.h \
    network/netbattleclient.h \
    network/netbattlehost.h \
    network/netfightwindow.h \
    ui/fightwindow.h \
    ui/mainwindow.h \
    ui/npcdialog.h \
    ui/pokemoninfo.h \
    ui/shopwindow.h

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    main.qrc \
    pokemon.qrc

# 子目录头文件统一通过 include path 解析，源文件中的 #include "xxx.h" 无需改路径
INCLUDEPATH += \
    $$PWD/common \
    $$PWD/data \
    $$PWD/battle \
    $$PWD/map \
    $$PWD/network \
    $$PWD/ui
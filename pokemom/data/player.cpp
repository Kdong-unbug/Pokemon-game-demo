#include "player.h"
#include "pokemon.h"
#include "item.h"
#include <QJsonArray>


Player::Player()
{
    playerName = "小明";
    money = 100;
    currentPokemonIndex = 0;

    // 初始化背包道具
    bag.addItem(ITEM_POTION, 5);
    bag.addItem(ITEM_POKEBALL, 3);
}

// ============= 队伍管理 =============
void Player::addPokemon(const Pokemon& pokemon)
{
    if (!isTeamFull()) {
        team.append(pokemon);
    } else {
        // 队伍已满，自动存入仓库
        addToStorage(pokemon);
    }
}

Pokemon& Player::getCurrentPokemon()
{
    if (team.isEmpty()) {
        static Pokemon emptyPoke;
        return emptyPoke;
    }
    return team[currentPokemonIndex];
}

void Player::setCurrentPokemon(int index)
{
    if (index >= 0 && index < team.size()) {
        currentPokemonIndex = index;
    }
}

bool Player::isTeamFull() const
{
    return team.size() >= 6;
}

int Player::getTeamSize() const
{
    return team.size();
}

bool Player::hasHealthyPokemon() const
{
    for (const Pokemon& p : team) {
        if (p.hp > 0)
            return true;
    }
    return false;
}

void Player::healAll()
{
    for (Pokemon& p : team) {
        p.hp = p.maxhp;
        p.status = StatusEffect::None;
        p.statusTurns = 0;
    }
    for (Pokemon& p : storage) {
        p.hp = p.maxhp;
        p.status = StatusEffect::None;
        p.statusTurns = 0;
    }
}

// ============= 仓库管理 =============
void Player::addToStorage(const Pokemon& pokemon)
{
    if (!isStorageFull()) {
        storage.append(pokemon);
    }
    // 如果仓库也满，可做进一步处理（如丢弃或提示），这里忽略
}

bool Player::isStorageFull() const
{
    return storage.size() >= 30;  // 仓库容量30只
}

int Player::getStorageSize() const
{
    return storage.size();
}

// ============= 金钱 =============
void Player::gainMoney(int amount)
{
    money += amount;
}

bool Player::spendMoney(int amount)
{
    if (money >= amount) {
        money -= amount;
        return true;
    }
    return false;
}

QJsonObject Player::toJson() const
{
    QJsonObject obj;
    obj["name"] = playerName;
    obj["money"] = money;
    obj["currentIndex"] = currentPokemonIndex;

    // 序列化队伍：把每只精灵转换成 JSON 对象，放入数组
    QJsonArray teamArray;
    for (const Pokemon& p : team)
    {
        teamArray.append(p.toJson());
    }
    obj["team"] = teamArray;

    // 序列化背包
    obj["bag"] = bag.toJson();

    // 序列化仓库（如果你想保存的话）
    QJsonArray storageArray;
    for (const Pokemon& p : storage)
    {
        storageArray.append(p.toJson());
    }
    obj["storage"] = storageArray;

    return obj;
}

void Player::fromJson(const QJsonObject& obj)
{
    playerName = obj["name"].toString();
    money = obj["money"].toInt();
    currentPokemonIndex = obj["currentIndex"].toInt();

    // 恢复队伍
    team.clear();
    QJsonArray teamArray = obj["team"].toArray();
    for (const QJsonValue& val : teamArray) {
        Pokemon p;
        p.fromJson(val.toObject());
        team.append(p);
    }

    // 防止旧存档中 currentPokemonIndex 越界导致后续崩溃
    if (team.isEmpty())
        currentPokemonIndex = 0;
    else if (currentPokemonIndex < 0 || currentPokemonIndex >= team.size())
        currentPokemonIndex = 0;

    // 恢复背包
    bag.fromJson(obj["bag"].toObject());

    // 恢复仓库
    storage.clear();
    QJsonArray storageArray = obj["storage"].toArray();
    for (const QJsonValue& val : storageArray) {
        Pokemon p;
        p.fromJson(val.toObject());
        storage.append(p);
    }
}

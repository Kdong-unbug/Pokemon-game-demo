#include "bag.h"

Bag::Bag() {}

void Bag::addItem(int itemId, int count)
{
    m_items[itemId] += count;
}

bool Bag::consumeItem(int itemId)
{
    if(!m_items.contains(itemId) || m_items[itemId] <=0)
        return false;
    m_items[itemId]--;
    if(m_items[itemId] <=0)
        m_items.remove(itemId);
    return true;
}

int Bag::getItemCount(int itemId)
{
    return m_items.value(itemId,0);
}

QList<int> Bag::getAllItemIds()
{
    return m_items.keys();
}

QJsonObject Bag::toJson() const
{
    QJsonObject itemsObj;
    // 遍历 map，把每个物品 ID 和数量写入 JSON 对象的属性
    // 因为 JSON 的 key 必须是字符串，所以把 int 转为 QString
    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        itemsObj[QString::number(it.key())] = it.value();
    }
    QJsonObject obj;
    obj["items"] = itemsObj;
    return obj;
}

void Bag::fromJson(const QJsonObject& obj)
{
    m_items.clear();
    QJsonObject itemsObj = obj["items"].toObject();
    // 遍历 JSON 对象的每个属性，还原成 int->int 的映射
    for (auto it = itemsObj.begin(); it != itemsObj.end(); ++it) {
        int id = it.key().toInt();
        int count = it.value().toInt();
        m_items[id] = count;
    }
}

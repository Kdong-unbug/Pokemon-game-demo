#ifndef BAG_H
#define BAG_H
#include <QMap>
#include <QJsonObject>   // 新增

class Bag
{
public:
    Bag();

    void addItem(int itemId, int count=1);
    bool consumeItem(int itemId);
    int getItemCount(int itemId);
    QList<int> getAllItemIds();




    // 序列化
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);

private:
    QMap<int,int> m_items; // <itemId, count>
};

#endif

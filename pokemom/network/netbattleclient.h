#ifndef NETBATTLECLIENT_H
#define NETBATTLECLIENT_H

#include <QTcpSocket>
#include <QJsonObject>
#include <functional>

class NetBattleClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit NetBattleClient(QObject *parent = nullptr);

    // 发送 JSON 消息
    void sendJson(const QJsonObject &obj);

signals:
    void jsonReceived(const QJsonObject &obj);   // 收到完整消息
    void connectedToHost();                      // 连接成功

private slots:
    void onReadyRead();

private:
    QByteArray buffer;  // 粘包缓存
};

#endif

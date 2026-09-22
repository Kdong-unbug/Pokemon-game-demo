#ifndef NETBATTLEHOST_H
#define NETBATTLEHOST_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <functional>

class NetBattleHost : public QTcpServer
{
    Q_OBJECT
public:
    explicit NetBattleHost(QObject *parent = nullptr);

    // 启动监听，返回是否成功
    bool startHost(quint16 port = 8000);

    // 向所有连接的客户端广播消息（通常只有两个）
    void broadcast(const QJsonObject &obj);

signals:
    void clientConnected(QTcpSocket *client);            // 有新客户端连接
    void clientDisconnected(QTcpSocket *client);         // 客户端断开
    void jsonReceived(QTcpSocket *sender, const QJsonObject &obj); // 收到消息

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onClientReadyRead();
    void onClientDisconnected();

private:
    QList<QTcpSocket*> m_clients;
};

#endif

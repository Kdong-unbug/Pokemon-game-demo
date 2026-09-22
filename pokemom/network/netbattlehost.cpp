#include "netbattlehost.h"
#include <QJsonDocument>
#include <QDebug>

NetBattleHost::NetBattleHost(QObject *parent) : QTcpServer(parent) {}

bool NetBattleHost::startHost(quint16 port)
{
    return listen(QHostAddress::Any, port);

}

void NetBattleHost::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    m_clients.append(socket);

    // 为每个客户端初始化独立的缓冲区，存储在 QObject 属性中
    socket->setProperty("buffer", QByteArray());

    connect(socket, &QTcpSocket::readyRead, this, &NetBattleHost::onClientReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &NetBattleHost::onClientDisconnected);

    emit clientConnected(socket);
}

void NetBattleHost::broadcast(const QJsonObject &obj)
{
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact) + '\n';
    for (QTcpSocket *s : m_clients) {
        if (s && s->state() == QAbstractSocket::ConnectedState) {
            s->write(data);
        }
    }
}

void NetBattleHost::onClientReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // 获取该 socket 专属的缓冲区
    QByteArray currentBuffer = socket->property("buffer").toByteArray();

    // 读取新数据并追加
    currentBuffer.append(socket->readAll());

    int pos;
    while ((pos = currentBuffer.indexOf('\n')) != -1) {
        QByteArray line = currentBuffer.left(pos);
        currentBuffer.remove(0, pos + 1);

        // 忽略空行
        if (line.isEmpty()) continue;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);

        if (!doc.isNull() && doc.isObject()) {
            emit jsonReceived(socket, doc.object());
        } else {
            qDebug() << "[Host] JSON Parse Error from client:" << parseError.errorString();
            qDebug() << "[Host] Raw Data:" << line;
        }
    }

    // 将剩余未处理的数据存回属性
    socket->setProperty("buffer", currentBuffer);
}

void NetBattleHost::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_clients.removeAll(socket);
        emit clientDisconnected(socket);
        socket->deleteLater();
    }
}

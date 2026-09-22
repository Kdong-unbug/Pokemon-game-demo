#include "netbattleclient.h"
#include <QJsonDocument>
#include <QJsonArray>

NetBattleClient::NetBattleClient(QObject *parent) : QTcpSocket(parent)
{
    connect(this, &QTcpSocket::readyRead, this, &NetBattleClient::onReadyRead);
    connect(this, &QTcpSocket::connected, this, &NetBattleClient::connectedToHost);
}

void NetBattleClient::sendJson(const QJsonObject &obj)
{
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact) + '\n';
    write(data);
}
void NetBattleClient::onReadyRead()
{
    buffer.append(readAll());
    int pos;
    while ((pos = buffer.indexOf('\n')) != -1)
    {
           QByteArray line = buffer.left(pos);
           buffer.remove(0,pos+1);
           QJsonDocument doc = QJsonDocument::fromJson(line);
           if(!doc.isNull())
           {
               emit jsonReceived(doc.object());
           }

}
}

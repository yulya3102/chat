#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QHostAddress>
class QUdpSocket;

class Receiver : public QObject
{
    Q_OBJECT
public:
    Receiver(const QHostAddress& bindTo);
    ~Receiver();

signals:
    void receivedHello(const QString& nickname, const QHostAddress& address);
    void receivedResponse(const QString& nickList, const QHostAddress& host);
    void receivedJoin(const QString& nickname, const QHostAddress& address);
    void receivedUnknownMessage(const QString& message);
    void receivedMessage(const QString& message, const QHostAddress& address);
    void receivedAccepted(const QString& message);
    void receivedQuit(const QHostAddress& who);
    void receivedGet(const QHostAddress& address);
    void receivedKeepalive(const QHostAddress& address);

private slots:
    void processPendingDatagrams();

private:
    void parseMessage(QString message, const QHostAddress &address);
    QUdpSocket *udpSocket;
};

#endif // RECEIVER_H

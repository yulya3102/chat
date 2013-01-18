#ifndef CHAT_H
#define CHAT_H

#include "receiver.h"
#include <QMap>
#include "nickname.h"

class QNetworkAddressEntry;
class UndeliveredMessage;
class QHostAddress;
class QTimer;

class Chat : public QObject
{
    Q_OBJECT
public:
    Chat(const QString& nickname, const QNetworkAddressEntry& networkAddressEntry);
    QString nicknameListToString() const;
    const QStringList& nicknameListToStringList() const;
    void addToHistory(const QString& timestamp, const QString& nickname, const QString &message);
    ~Chat();
public slots:
    void handleHello(const QString& nickname, const QHostAddress& address);
    void parseResponse(QString nickList, const QHostAddress &host);
    void addNew(const QString& nickname, const QHostAddress& address);
    void sendMessage(const QString& message);
    void sendToAllNicknames(const QString& message, const QSet<QHostAddress> &ignoreList);
    void sendToAllNicknames(const QString& message);
    void parseMessage(const QString& message, const QHostAddress& address);
    void handleAccepted(QString message);
    void quit();
    void handleQuit(QHostAddress who);
    void sendResponse(const QHostAddress &address);
    void handleKeepalive(const QHostAddress& address);
private slots:
    void timerHelloTimeout();
    void timerKeepaliveTimeout();
    void handleDead(const QHostAddress& who);
signals:
    void nicknameListChanged(const QStringList& nicknameList);
    void sendMessage(const QString& message, const QHostAddress& address);
    void newChatMessage(const QString& message);
    void sendHello(const QString& message);
private:
    QString getNicknameListKey(const QHostAddress& address);
    Nickname* getNickname(const QHostAddress& address);
    QMap<QString, Nickname*> _nicknameList;
    QString _myNickname;
    const QNetworkAddressEntry *_networkAddressEntry;
    QMap<QString, QString> *_chatHistory;
    QMap<QString, UndeliveredMessage*> *_undeliveredMessages;
    QTimer *_timer;
};

#endif // CHAT_H

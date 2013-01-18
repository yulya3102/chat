#include "chat.h"
#include <iostream>
#include "cmd.h"
#include "sender.h"
#include "time.h"
#include "undeliveredmessage.h"
#include <ctime>
#include <QtAlgorithms>

Chat::Chat(const QString &nickname, const QNetworkAddressEntry &networkAddressEntry) :
    _myNickname(nickname),
    _networkAddressEntry(&networkAddressEntry),
    _chatHistory(new QMap<QString, QString>),
    _undeliveredMessages(new QMap<QString, UndeliveredMessage*>),
    _timer(new QTimer())
{
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(timerHelloTimeout()));
    _timer->start(CMD::HELLO_TIMEOUT);
}

QString Chat::nicknameListToString() const
{
    QString result;
    for (QMap<QString, Nickname*>::ConstIterator i = _nicknameList.constBegin(); i != _nicknameList.constEnd(); i++)
        result += " " + (*i)->address().toString() + " " + i.key();
    return result;
}

const QStringList &Chat::nicknameListToStringList() const
{
    QStringList * result = new QStringList();
    for (QMap<QString, Nickname*>::ConstIterator i = _nicknameList.constBegin(); i != _nicknameList.constEnd(); i++)
        result->push_back("[" + (*i)->address().toString() + "] " + i.key());
    return *result;
}

void Chat::addToHistory(const QString &timestamp, const QString& nickname, const QString &message)
{
    _chatHistory->insert(timestamp + nickname, message);
    time_t time = timestamp.toInt();
    char date[10];
    strftime(date, 10, "%H:%M:%S", localtime(&time));
    emit newChatMessage("[" + QString(date) + " " + nickname + "]: " + message);
}

Chat::~Chat()
{
    delete _chatHistory; _chatHistory = NULL;
    std::cout << "ChatHistory deleted" << std::endl;
    qDeleteAll(*_undeliveredMessages);
    _undeliveredMessages->clear();
    delete _undeliveredMessages; _undeliveredMessages = NULL;
    std::cout << "UndeliveredMessages deleted" << std::endl;
    _timer->stop(); delete _timer; _timer = NULL;
    std::cout << "Chat deleted" << std::endl;
}

void Chat::handleHello(const QString &nickname, const QHostAddress &address)
{
    addNew(nickname, address);
    sendResponse(address);
}

void Chat::parseResponse(QString nickList, const QHostAddress& host)
{
    QString hostNickname;
    if (nickList.contains(" "))
    {
        hostNickname = nickList.left(nickList.indexOf(" "));
        nickList.remove(0, nickList.indexOf(" ") + 1);
    }
    else
    {
        hostNickname = nickList;
    }
    if (hostNickname.length() == 0)
    {
        std::cout << "Invalid RESPONSE!" << std::endl;
        return;
    }
    addNew(hostNickname, host);
    while (nickList.contains(" "))
    {
        QString address = nickList.left(nickList.indexOf(" "));
        nickList.remove(0, nickList.indexOf(" ") + 1);
        QString nickname;
        if (nickList.contains(" "))
        {
            nickname = nickList.left(nickList.indexOf(" "));
            nickList.remove(0, nickList.indexOf(" ") + 1);
        }
        else
        {
            nickname = nickList;
        }
        if (nickname.length() == 0 || address.length() == 0)
        {
            std::cout << "Invalid RESPONSE!" << std::endl;
            return;
        }
        addNew(nickname, QHostAddress(address));
    }
}


void Chat::addNew(const QString& nickname, const QHostAddress& address)
{
    if (nickname != _myNickname)
    {
        if (_nicknameList.find(nickname) == _nicknameList.end())
        {
            std::cout << "Adding new nickname '" << nickname.toStdString() << "' to nicklist with IP " << address.toString().toStdString() << std::endl;
            sendToAllNicknames(CMD::JOIN + " " + address.toString() + " " + nickname);
            _nicknameList.insert(nickname, new Nickname(address));
            QObject::connect(_nicknameList[nickname], SIGNAL(dead(QHostAddress)), this, SLOT(handleDead(QHostAddress)));
            emit newChatMessage("Join: " + nickname);
            emit nicknameListChanged(nicknameListToStringList());
        }
        else
        {
            (*_nicknameList.find(nickname))->resetTimer();
        }
    }
}

void Chat::sendMessage(const QString &message)
{
    QString timestamp = QString::number(time(NULL));
    QString resultMessage = CMD::MESSAGE + " " + _networkAddressEntry->ip().toString() + " " + _myNickname + " " + timestamp + " " + message + "\0";
    sendToAllNicknames(resultMessage);
    addToHistory(timestamp, _myNickname, message);
    UndeliveredMessage *newMessage = new UndeliveredMessage(resultMessage);
    QObject::connect(newMessage, SIGNAL(resend(QString)), this, SLOT(sendToAllNicknames(QString)));
    _undeliveredMessages->insert(timestamp + _myNickname, newMessage);
}

void Chat::sendToAllNicknames(const QString &message, const QSet<QHostAddress>& ignoreList)
{
    std::cout << "Sending to all nicknames message ignoring some '" << message.toStdString() << "'" << std::endl;
    for (QMap<QString, Nickname*>::ConstIterator it = _nicknameList.begin(); it != _nicknameList.end(); it++)
    {
        if (!ignoreList.contains((*it)->address()))
            emit sendMessage(message, (*it)->address());
    }
}

void Chat::sendToAllNicknames(const QString &message)
{
    std::cout << "Sending to all nicknames message '" << message.toStdString() << "'" << std::endl;
    for (QMap<QString, Nickname*>::ConstIterator it = _nicknameList.begin(); it != _nicknameList.end(); it++)
    {
        //std::cout << "Sending message '" << message.toStdString() << "' to " << (*it)->address().toString().toStdString() << std::endl;
        emit sendMessage(message, (*it)->address());
    }
}

QString Chat::getNicknameListKey(const QHostAddress& address)
{
    QString key = CMD::DEFAULT_KEY;
    for (QMap<QString, Nickname*>::ConstIterator it = _nicknameList.begin(); it != _nicknameList.end(); it++)
        if ((*it)->address() == address)
            key = it.key();
    return key;
}

Nickname *Chat::getNickname(const QHostAddress &address)
{
    for (QMap<QString, Nickname*>::ConstIterator it = _nicknameList.begin(); it != _nicknameList.end(); it++)
        if ((*it)->address() == address)
            return *it;
    return NULL;
}

void Chat::parseMessage(const QString &message, const QHostAddress &address)
{
    std::cout << ("Parsing message 'MESSAGE " + message + "' from " + address.toString()).toStdString() << std::endl;
    QString messageToParse = message;
    if (getNicknameListKey(address) != CMD::DEFAULT_KEY)
    {
        QString ip = messageToParse.left(messageToParse.indexOf(" "));
        messageToParse.remove(0, messageToParse.indexOf(" ") + 1);
        QString nickname = messageToParse.left(messageToParse.indexOf(" "));
        messageToParse.remove(0, messageToParse.indexOf(" ") + 1);
        QString timestamp = messageToParse.left(messageToParse.indexOf(" "));
        messageToParse.remove(0, messageToParse.indexOf(" ") + 1);
        if (ip.length() == 0 || nickname.length() == 0 || timestamp.length() == 0)
        {
            std::cout << "Invalid MESSAGE!" << std::endl;
            return;
        }
        if (getNicknameListKey(QHostAddress(ip)) == CMD::DEFAULT_KEY)
            return;
        if (!_chatHistory->contains(timestamp + nickname))
        {
            addToHistory(timestamp, nickname, messageToParse);
            QSet<QHostAddress> ignoreList;
            ignoreList.insert(address);
            ignoreList.insert(QHostAddress(ip));
            sendToAllNicknames(CMD::MESSAGE + " " + message, ignoreList);
            UndeliveredMessage *newMessage = new UndeliveredMessage(CMD::MESSAGE + " " + message, ignoreList);
            QObject::connect(newMessage, SIGNAL(resend(QString,QSet<QHostAddress>)), this, SLOT(sendToAllNicknames(QString,QSet<QHostAddress>)));
            _undeliveredMessages->insert(timestamp + _myNickname, newMessage);
        }
        emit sendMessage(CMD::ACCEPTED + " " + nickname + " " + timestamp, address);
    }
}

void Chat::handleAccepted(QString message)
{
    QString nickname = message.left(message.indexOf(" "));
    message.remove(0, message.indexOf(" ") + 1);
    QString timestamp = message;
    if (nickname.length() == 0 || timestamp.length() == 0 || message.indexOf(" ") != -1)
    {
        std::cout << "Invalid ACCEPTED!" << std::endl;
        return;
    }
    std::cout << "Deleting delivered message from nickname '" << nickname.toStdString() << "' and timestamp '" << timestamp.toStdString() << "'" << std::endl;
    delete (*_undeliveredMessages)[timestamp + nickname]; (*_undeliveredMessages)[timestamp + nickname] = NULL;
    _undeliveredMessages->remove(timestamp + nickname);
}

void Chat::quit()
{
    std::cout << "Quit" << std::endl;
    QString message = CMD::QUIT + " " + _networkAddressEntry->ip().toString();
    sendToAllNicknames(message);
}

void Chat::handleQuit(QHostAddress who)
{
    std::cout << "Quits: " << who.toString().toStdString() << std::endl;
    QString key = getNicknameListKey(who);
    if (key != CMD::DEFAULT_KEY)
    {
        delete _nicknameList[key]; _nicknameList[key] = NULL;
        _nicknameList.remove(key);
        emit newChatMessage("Quit: " + key);
        emit nicknameListChanged(nicknameListToStringList());
        sendToAllNicknames(CMD::QUIT + " " + who.toString());
    }
}

void Chat::sendResponse(const QHostAddress &address)
{
    emit sendMessage(CMD::RESPONSE + " " + _myNickname + this->nicknameListToString(), address);
}

void Chat::handleKeepalive(const QHostAddress &address)
{
    Nickname * nickname = getNickname(address);
    if (nickname != NULL)
        nickname->resetTimer();
}

void Chat::timerHelloTimeout()
{
    if (_nicknameList.size() == 0)
        emit sendHello(CMD::HELLO + " " + _myNickname);
    else
    {
        _timer->stop();
        QObject::disconnect(_timer, SIGNAL(timeout()), this, SLOT(timerHelloTimeout()));
        QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(timerKeepaliveTimeout()));
        _timer->start(CMD::KEEPALIVE_TIMEOUT);
    }
}

void Chat::timerKeepaliveTimeout()
{
    sendToAllNicknames(CMD::KEEPALIVE);
}

void Chat::handleDead(const QHostAddress &who)
{
    std::cout << "Oops, " << who.toString().toStdString() << " is dead :(" << std::endl;
    handleQuit(who);
}

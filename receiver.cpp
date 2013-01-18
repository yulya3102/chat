#include "receiver.h"
#include "cmd.h"
#include <QtNetwork>
#include <QNetworkInterface>
#include <iostream>

Receiver::Receiver(const QHostAddress& bindTo) :
    udpSocket(new QUdpSocket())
{
    udpSocket->bind(bindTo, CMD::PORT);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

Receiver::~Receiver()
{
    delete udpSocket; udpSocket = NULL;
    std::cout << "Receiver deleted" << std::endl;
}


void Receiver::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress sender;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender);
        QString message = datagram.data();
        int index = message.lastIndexOf("\r\n");
        message.remove(index, message.size() - index);
        parseMessage(message, sender);
    }
}

void Receiver::parseMessage(QString message, const QHostAddress& address)
{
    QString command = message.left(message.indexOf(" "));
    if (QString::compare(command, CMD::HELLO) == 0)
    {
        message.remove(0, message.indexOf(" ") + 1);
        std::cout << "Received 'HELLO " << message.toStdString() << "' from " << address.toString().toStdString() << std::endl;
        if (message.length() == 0 || message.indexOf(" ") != -1)
        {
            std::cout << "Invalid HELLO!" << std::endl;
            return;
        }
        emit receivedHello(message, address);
    }
    else if (QString::compare(command, CMD::RESPONSE) == 0)
    {
        message.remove(0, message.indexOf(" ") + 1);
        std::cout << "Received 'RESPONSE " << message.toStdString() << "' from " << address.toString().toStdString() << std::endl;
        emit receivedResponse(message, address);
    }
    else if (QString::compare(command, CMD::JOIN) == 0)
    {
        message.remove(0, message.indexOf(" ") + 1);
        std::cout << "Received 'JOIN " << message.toStdString() << "' from " << address.toString().toStdString() << std::endl;
        QString address = message.left(message.indexOf(" "));
        message.remove(0, message.indexOf(" ") + 1);
        QString nickname = message;
        if (address.length() == 0 || nickname.length() == 0)
        {
            std::cout << "Invalid JOIN!" << std::endl;
            return;
        }
        emit receivedJoin(nickname, QHostAddress(address));
    }
    else if (QString::compare(command, CMD::MESSAGE) == 0)
    {
        message.remove(0, message.indexOf(" ") + 1);
        std::cout << "Received 'MESSAGE " << message.toStdString() << "' from " << address.toString().toStdString() << std::endl;
        emit receivedMessage(message, address);
    }
    else if (QString::compare(command, CMD::ACCEPTED) == 0)
    {
        message.remove(0, message.indexOf(" ") + 1);
        std::cout << "Received 'ACCEPTED " << message.toStdString() << "' from " << address.toString().toStdString() << std::endl;
        if (message.length() == 0 || message.indexOf(" ") == -1)
        {
            std::cout << "Invalid ACCEPTED!" << std::endl;
            return;
        }
        emit receivedAccepted(message);
    }
    else if (QString::compare(command, CMD::QUIT) == 0)
    {
        message.remove(0, message.indexOf(" ") + 1);
        std::cout << "Received 'QUIT " << message.toStdString() << "' from " << address.toString().toStdString() << std::endl;
        if (message.length() == 0 || message.indexOf(" ") != -1)
        {
            std::cout << "Invalid QUIT!" << std::endl;
            return;
        }
        emit receivedQuit(QHostAddress(message));
    }
    else if (QString::compare(command, CMD::GET) == 0)
    {
        std::cout << "Received 'GET' from " << address.toString().toStdString() << std::endl;
        emit receivedGet(address);
    }
    else if (QString::compare(command, CMD::KEEPALIVE) == 0)
    {
        std::cout << "Received 'KEEPALIVE' from " << address.toString().toStdString() << std::endl;
        emit receivedKeepalive(address);
    }
    else
    {
        std::cout << "Recieved unknown message '" << message.toStdString() << "'" << std::endl;
        emit receivedUnknownMessage(QString("Received message \"%1\" from %2").arg(message, address.toString()));
    }
}

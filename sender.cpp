#include <QtNetwork>

#include "sender.h"
#include "cmd.h"
#include <iostream>


SelectInterfaceDialog::SelectInterfaceDialog(QWidget *parent, char const* name) : QDialog(parent)
{
    listWidget = new QListWidget(this);
    list = new QList<QNetworkAddressEntry>();
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (QList<QNetworkInterface>::Iterator i = interfaces.begin(); i != interfaces.end(); i++)
    {
        QList<QNetworkAddressEntry> addresses = i->addressEntries();
        for (QList<QNetworkAddressEntry>::Iterator j = addresses.begin(); j != addresses.end(); j++)
        {
            list->push_back(*j);
            listWidget->addItem(list->back().ip().toString());
        }
    }
    button = new QPushButton("OK", this);
    connect(button, SIGNAL(clicked()), this, SLOT(returnInterface()));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(listWidget);
    layout->addWidget(button);
    layout->addStretch(1);
}

SelectInterfaceDialog::~SelectInterfaceDialog()
{
    delete listWidget; listWidget = NULL;
    delete button; button = NULL;
    delete list; list = NULL;
    std::cout << "SelectInterfaceDialog deleted" << std::endl;
}

void SelectInterfaceDialog::returnInterface()
{
    int index = listWidget->currentIndex().row();
    emit interfaceSelected(list->at(index));
    close();
}

Sender::Sender() :
    udpSocket(new QUdpSocket()),
    port(CMD::PORT)
{
    SelectInterfaceDialog dialog;
    QObject::connect(&dialog, SIGNAL(interfaceSelected(QNetworkAddressEntry)), this, SLOT(setInterface(QNetworkAddressEntry)));
    dialog.exec();
}

Sender::~Sender()
{
    delete udpSocket; udpSocket = NULL;
    std::cout << "Sender deleted" << std::endl;
}

const QNetworkAddressEntry &Sender::getNetworkAddressEntry() const
{
    return networkAddressEntry;
}

void Sender::sendBroadcastMessage(const QString& message)
{
    QByteArray datagram = (message + "\r\n").toUtf8();
    udpSocket->writeDatagram(datagram.data(), datagram.size(), networkAddressEntry.broadcast(), port);
    //std::cout << "Sending broadcast message '" << message.toStdString() << "' to " << networkAddressEntry.broadcast().toString().toStdString() << std::endl;
}

void Sender::sendMessageToHost(const QString &message, const QHostAddress &host)
{
    if (host != udpSocket->peerAddress())
        udpSocket->connectToHost(host, port);
    QByteArray datagram = (message + "\r\n").toUtf8();
    udpSocket->writeDatagram(datagram.data(), datagram.size(), host, port);
    //std::cout << "Sending message '" << message.toStdString() << "' to " << host.toString().toStdString() << std::endl;
    udpSocket->disconnectFromHost();
}

void Sender::setInterface(const QNetworkAddressEntry &networkAddressEntry)
{
    std::cout << "Set interface " << networkAddressEntry.ip().toString().toStdString() << std::endl;
    this->networkAddressEntry = networkAddressEntry;
}

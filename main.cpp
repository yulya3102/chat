#include <QApplication>
#include "mainwindow.h"
#include "sender.h"
#include "receiver.h"
#include "cmd.h"
#include "chat.h"
#include <QInputDialog>
#include <iostream>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString nickname;
    bool nicknameIsInvalid = true;
    QRegExp regexp("[^A-Z][^a-z][^0-9][^-_.]");
    do {
        nickname = QInputDialog::getText(NULL, "Select your nickname", "Enter your nickname:", QLineEdit::Normal, "yulya3102");
        if (regexp.indexIn(nickname) == -1)
            nicknameIsInvalid = false;
        if (nicknameIsInvalid) {
            QMessageBox::warning(NULL, "Select your nickname", "Nickname is invalid!");
        }

    } while (nicknameIsInvalid);
    Receiver receiver(QHostAddress::Any);
    Sender sender;
    Chat chat(nickname, sender.getNetworkAddressEntry());
    MainWindow w;
    QObject::connect(&receiver, SIGNAL(receivedHello(QString,QHostAddress)), &chat, SLOT(handleHello(QString,QHostAddress)));
    QObject::connect(&receiver, SIGNAL(receivedJoin(QString,QHostAddress)), &chat, SLOT(addNew(QString,QHostAddress)));
    QObject::connect(&chat, SIGNAL(sendMessage(QString,QHostAddress)), &sender, SLOT(sendMessageToHost(QString,QHostAddress)));
    QObject::connect(&receiver, SIGNAL(receivedResponse(QString, QHostAddress)), &chat, SLOT(parseResponse(QString, QHostAddress)));
    QObject::connect(&w, SIGNAL(sendMessage(QString)), &chat, SLOT(sendMessage(QString)));
    QObject::connect(&chat, SIGNAL(nicknameListChanged(QStringList)), &w, SLOT(changeNicknameList(QStringList)));
    QObject::connect(&chat, SIGNAL(newChatMessage(QString)), &w, SLOT(addMessage(QString)));
    QObject::connect(&receiver, SIGNAL(receivedMessage(QString, QHostAddress)), &chat, SLOT(parseMessage(QString, QHostAddress)));
    QObject::connect(&receiver, SIGNAL(receivedAccepted(QString)), &chat, SLOT(handleAccepted(QString)));
    QObject::connect(&w, SIGNAL(quit()), &chat, SLOT(quit()));
    QObject::connect(&receiver, SIGNAL(receivedQuit(QHostAddress)), &chat, SLOT(handleQuit(QHostAddress)));
    QObject::connect(&receiver, SIGNAL(receivedGet(QHostAddress)), &chat, SLOT(sendResponse(QHostAddress)));
    QObject::connect(&chat, SIGNAL(sendHello(QString)), &sender, SLOT(sendBroadcastMessage(QString)));
    QObject::connect(&receiver, SIGNAL(receivedKeepalive(QHostAddress)), &chat, SLOT(handleKeepalive(QHostAddress)));
    sender.sendBroadcastMessage(CMD::HELLO + " " + nickname);
    w.addMessage("Hello, " + nickname);
    w.show();
    return a.exec();
}

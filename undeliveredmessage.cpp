#include "undeliveredmessage.h"
#include <QTimer>
#include "cmd.h"
#include <iostream>

UndeliveredMessage::UndeliveredMessage(const QString &message) :
    message(message),
    timer(new QTimer()),
    ignoreList(NULL)
{
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    timer->start(CMD::MESSAGE_TIMEOUT);
}

UndeliveredMessage::UndeliveredMessage(const QString &message, const QSet<QHostAddress> &ignoreList) :
    message(message),
    timer(new QTimer()),
    ignoreList(&ignoreList)
{
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    timer->start(CMD::MESSAGE_TIMEOUT);
}

UndeliveredMessage::~UndeliveredMessage()
{
    timer->stop();
    delete timer; timer = NULL;
    //delete ignoreList; ignoreList = NULL;
    std::cout << "UndeliveredMessage deleted" << std::endl;
}

void UndeliveredMessage::timerTimeout()
{
    emit resend(message);
}

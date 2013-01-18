#include "nickname.h"
#include <QObject>
#include "cmd.h"
#include <iostream>

Nickname::Nickname(const QHostAddress &address) :
    _address(address),
    _timer()
{
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    _timer.start(CMD::KEEPALIVE_CLIENT_TIMEOUT);
}

const QHostAddress &Nickname::address() const
{
    return _address;
}

bool Nickname::operator==(const Nickname &other)
{
    return _address == other._address;
}

void Nickname::resetTimer()
{
    std::cout << "Reset keepalive timer from " << _address.toString().toStdString() << std::endl;
    _timer.stop();
    _timer.start(CMD::KEEPALIVE_CLIENT_TIMEOUT);
}

void Nickname::timerTimeout()
{
    emit dead(_address);
}

#ifndef NICKNAME_H
#define NICKNAME_H

#include <QObject>
#include <QHostAddress>
#include <QTimer>

class Nickname : public QObject
{
    Q_OBJECT
public:
    Nickname(const QHostAddress& address);
    const QHostAddress& address() const;
    bool operator==(const Nickname& other);
    void resetTimer();
private slots:
    void timerTimeout();
signals:
    void dead(const QHostAddress& address);
private:
    QHostAddress _address;
    QTimer _timer;
};

#endif // NICKNAME_H

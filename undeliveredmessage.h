#ifndef UNDELIVEREDMESSAGE_H
#define UNDELIVEREDMESSAGE_H
#include <QObject>


class QTimer;
class QHostAddress;
class Nickname;

class UndeliveredMessage : public QObject
{
    Q_OBJECT
public:
    UndeliveredMessage(const QString& message);
    UndeliveredMessage(const QString &message, const QSet<QHostAddress>& ignoreList);
    ~UndeliveredMessage();
signals:
    void resend(const QString& message);
    void resend(const QString& message, const QSet<QHostAddress>& ignoreList);
public slots:
    void timerTimeout();
private:
    QString message;
    QTimer *timer;
    const QSet<QHostAddress> *ignoreList;
};

#endif // UNDELIVEREDMESSAGE_H

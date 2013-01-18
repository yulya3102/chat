#ifndef CMD_H
#define CMD_H

#include <QString>

class CMD
{
public:
    static const QString HELLO;
    static const QString RESPONSE;
    static const QString JOIN;
    static const QString MESSAGE;
    static const QString ACCEPTED;
    static const QString QUIT;
    static const QString GET;
    static const QString KEEPALIVE;
    static const int PORT = 3141;
    //static const int PORT = 45454;
    static const int MESSAGE_TIMEOUT = 60 * 1000;
    //static const int MESSAGE_TIMEOUT = 1000;
    static const QString DEFAULT_KEY;
    static const int HELLO_TIMEOUT = 30 * 1000;
    static const int KEEPALIVE_TIMEOUT = 2 * 60 * 1000;
    static const int KEEPALIVE_CLIENT_TIMEOUT = 10 * 60 * 1000;
    //static const int KEEPALIVE_CLIENT_TIMEOUT = 60 * 1000;
    static const int MIN_MESSAGES_INTERVAL = 1000;
private:
    CMD();
};

#endif // CMD_H

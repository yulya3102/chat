#ifndef SENDER_H
#define SENDER_H

#include <QtNetwork>

 class QUdpSocket;

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
 class SelectInterfaceDialog : public QDialog
 {
     Q_OBJECT
 public:
     SelectInterfaceDialog(QWidget *parent = 0, char const* name = 0);
     ~SelectInterfaceDialog();
 signals:
     void interfaceSelected(const QNetworkAddressEntry&);
 public slots:
     void returnInterface();
 private:
     QListWidget *listWidget;
     QPushButton *button;
     QList<QNetworkAddressEntry> *list;
 };


 class Sender : public QObject
 {
     Q_OBJECT

 public:
     Sender();
     ~Sender();
     const QNetworkAddressEntry& getNetworkAddressEntry() const;

 public slots:
     void sendBroadcastMessage(const QString& message);
     void sendMessageToHost(const QString& message, const QHostAddress& host);
     void setInterface(const QNetworkAddressEntry& networkAddressEntry);

 private:
     QUdpSocket *udpSocket;
     quint16 port;
     QNetworkAddressEntry networkAddressEntry;
 };

#endif // SENDER_H

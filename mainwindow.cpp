#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QTextCodec>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>
#include "cmd.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    ui->setupUi(this);
    ui->editMessage->installEventFilter(this);
    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(sendButtonPressed()));
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
}

MainWindow::~MainWindow()
{
    emit quit();
    delete ui; ui = NULL;
    std::cout << "MainWindow deleted" << std::endl;
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->editMessage) {
        if (event->type() == QEvent::KeyPress) {
            if (((QKeyEvent *) event)->key() == Qt::Key_Return || ((QKeyEvent *) event)->key() == Qt::Key_Enter) {
                sendButtonPressed();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::addMessage(const QString & message)
{
    ui->chatList->append(message);
    ui->chatList->moveCursor(QTextCursor::End);
}

void MainWindow::changeNicknameList(const QStringList &nicknameList)
{
    ui->nicknameList->clear();
    ui->nicknameList->addItems(nicknameList);
}

void MainWindow::sendButtonPressed()
{
    QString message = QString(ui->editMessage->text().toUtf8());
    if (QString::compare(message, QString("")) != 0)
    {
        emit sendMessage(message);
        ui->editMessage->setDisabled(true);
        timer->start(CMD::MIN_MESSAGES_INTERVAL);
    }
    ui->editMessage->clear();
}

void MainWindow::timerTimeout()
{
    timer->stop();
    ui->editMessage->setDisabled(false);
    ui->editMessage->setFocus();
}

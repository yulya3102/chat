#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTimer;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool eventFilter(QObject *target, QEvent *event);

public slots:
    void addMessage(const QString& message);
    void changeNicknameList(const QStringList &nicknameList);

private slots:
    void sendButtonPressed();
    void timerTimeout();

signals:
    void sendMessage(const QString& message);
    void quit();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
};

#endif // MAINWINDOW_H

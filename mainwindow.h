#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void replyFinished(QNetworkReply*);

    void on_btBrowse_released();

    void on_btUpload_released();

private:
    Ui::MainWindow *ui;
    QString path;
    QNetworkAccessManager *manager;
    bool first = true;
    QString apiKey = "YOUR API KEY";
};

#endif // MAINWINDOW_H

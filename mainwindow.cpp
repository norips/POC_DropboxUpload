#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::replyFinished(QNetworkReply *nr) {
    if(first){
    QByteArray output = nr->readAll();
    qInfo(output);
    QJsonDocument doc = QJsonDocument::fromJson(output);
    QString formattedJsonString = doc.toJson(QJsonDocument::Indented);

    ui->teOutput->setText(formattedJsonString);

    QNetworkRequest nr(QUrl("https://api.dropboxapi.com/2/sharing/create_shared_link_with_settings"));
    nr.setRawHeader("Authorization",(tr("Bearer ") + apiKey).toUtf8());
    QString dropboxarg;
    dropboxarg.append("{\"path\": \"");
    QJsonObject jsonObject = doc.object();
    QString pathDrop = jsonObject["path_display"].toString();
    dropboxarg.append(pathDrop);
    dropboxarg.append("\",\"settings\": {\"requested_visibility\": \"public\"}}");
    nr.setRawHeader("Content-Type","application/json");
    manager->post(nr, dropboxarg.toUtf8());
    first = false;
    } else {
        QByteArray output = nr->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(output);
        QString formattedJsonString = doc.toJson(QJsonDocument::Indented);
        QString tex;
        tex.append(ui->teOutput->toPlainText());
        tex.append(formattedJsonString);
        tex.append("URL:\n");
        QString url = doc.object()["url"].toString();
        url[url.size()-1] = '1';
        tex.append(url);
        ui->teOutput->setText(tex);
    }
}

void MainWindow::on_btBrowse_released()
{
    path = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QDir::currentPath(),
                                                    tr("Files (*.*)"));
    ui->lEpath->setText(path);
}

void MainWindow::on_btUpload_released()
{
    if(path != NULL) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
                return;
        QByteArray data = file.readAll();
        QNetworkRequest nr(QUrl("https://content.dropboxapi.com/2/files/upload"));
        nr.setRawHeader("Authorization",(tr("Bearer ") + apiKey).toUtf8());
        QString dropboxarg;
        QFileInfo fileInfo(file.fileName());
        QString filename(fileInfo.fileName());
        dropboxarg.append("{\"path\": \"/");
        dropboxarg.append(filename);
        dropboxarg.append("\",\"mode\": \"overwrite\",\"mute\": false}");
        nr.setRawHeader("Dropbox-API-Arg",dropboxarg.toUtf8());
        nr.setRawHeader("Content-Type","application/octet-stream");
        manager->post(nr, data);
    }
}

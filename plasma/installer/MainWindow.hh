//
// ---------- header ----------------------------------------------------------
//
// project       infinit/plasma
//
// license       infinit
//
// author        Raphaël Londeix   [Thu 26 Jan 2012 07:05:56 PM CET]
//

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

//
// ---------- includes --------------------------------------------------------
//

#include <QFile>
#include <QMainWindow>
#include <QNetworkAccessManager>

namespace Ui {
class MainWindow;
}

//
// ---------- classes ---------------------------------------------------------
//


///
/// Since the task is really simple, this class
/// does everything :
///     - Ask a user to agree with the license terms
///     - download the latest updater
///     - launch it
///
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow*         _ui;
    QNetworkAccessManager*  _http;
    QNetworkReply*          _reply;
    QFile*                  _dest_file;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void on_launchInstall_clicked();
    void on_cancelInstall_clicked();

    //custom slots cannot start with "on_"
    void download_done(QNetworkReply*);
    void download_progress(qint64, qint64);
    void can_read();
};

#endif // MAINWINDOW_HH


#include <QMessageBox>

#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _userAgreed(false)
{
    this->_ui->setupUi(this);
    connect(this->_ui->launch_install, SIGNAL(clicked()), this, SLOT(_onLaunchInstall()));
    connect(this->_ui->cancel_install, SIGNAL(clicked()), this, SLOT(_onCancelInstall()));
    connect(this->_ui->user_agreement, SIGNAL(toggled(bool)), this, SLOT(_onUserAgree(bool)));
}

MainWindow::~MainWindow()
{
    delete this->_ui;
}


void MainWindow::_onCancelInstall()
{
    exit(EXIT_FAILURE);
}

void MainWindow::_onLaunchInstall()
{
    if (!this->_userAgreed)
    {
        QMessageBox::warning(this, "Cannot continue !", "You didn't agree the license terms");
        return;
    }
}

void MainWindow::_onUserAgree(bool f)
{
    this->_userAgreed = f;
}

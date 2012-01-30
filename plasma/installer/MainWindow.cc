
#include <cassert>
#include <iostream>

#include <QMessageBox>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include "resources.hh"

#include "MainWindow.hh"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  _ui(new Ui::MainWindow),
  _http(0),
  _reply(0),
  _dest_file(0)
{
  this->_ui->setupUi(this);
  this->_ui->downloadBar->setVisible(false);
}

MainWindow::~MainWindow()
{
  delete this->_http;
  this->_http = 0;
  delete this->_ui;
  this->_ui = 0;
  delete this->_dest_file;
  this->_dest_file = 0;
  delete this->_reply;
  this->_reply = 0;
}

void MainWindow::on_launchInstall_clicked()
{
  if (!this->_ui->userAgreement->isChecked())
    {
      QMessageBox::warning(this, "Cannot continue !", "You didn't agree the license terms");
      return;
    }

  // Prepare destination directory
  QDir(QDir::homePath()).mkdir(INFINIT_HOME);
  QDir dest_dir = QDir::homePath() + QDir::separator() + INFINIT_HOME;
  if (!dest_dir.exists())
    {
      QMessageBox::warning(this, "Cannot continue !", "Cannot create the destination directory " + dest_dir.path());
      return;
    }

  // Prepare destination file
  this->_dest_file = new QFile(dest_dir.path() + QDir::separator() + INFINIT_UPDATER_FILENAME);
  if (this->_dest_file->exists())
    this->_dest_file->remove();
  if (!this->_dest_file->open(QIODevice::WriteOnly))
    {
      QMessageBox::warning(this, "Cannot continue !", "Cannot create the destination file " + this->_dest_file->fileName());
      delete this->_dest_file;
      this->_dest_file = 0;
    }

  // Prepare GUI
  this->_ui->launchInstall->setDisabled(true);
  this->_ui->userAgreement->hide();
  this->_ui->userAgreementLabel->setText("Downloading files ...");
  this->_ui->downloadBar->setVisible(true);
  this->_ui->downloadBar->setMinimum(0);
  this->_ui->downloadBar->setMaximum(10000);
  this->_ui->downloadBar->setValue(0);

  assert(this->_http == 0);

  // Download the file
  this->_http = new QNetworkAccessManager(this);
  this->connect(this->_http, SIGNAL(finished(QNetworkReply*)), SLOT(download_done(QNetworkReply*)));

  this->_reply = this->_http->get(QNetworkRequest(QUrl(INFINIT_UPDATER_URI)));
  this->connect(this->_reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(download_progress(qint64, qint64)));
  this->connect(this->_reply, SIGNAL(readyRead()), SLOT(can_read()));
}

void MainWindow::download_progress(qint64 read, qint64 total)
{
  if (read <= 0 || total <= 0)
    {
      this->_ui->downloadBar->setValue(0);
      return;
    }
  int res = float(read) * 10000.0f / float(total);
  this->_ui->downloadBar->setValue(res);
}

void MainWindow::download_done(QNetworkReply* reply)
{
  assert(reply != 0);
  assert(reply == this->_reply);
  assert(this->_dest_file != 0);
  this->_dest_file->close();
  this->_dest_file->setPermissions(QFile::ExeOwner | QFile::ExeUser);
  this->_reply->deleteLater();
  this->_reply = 0;
  this->_ui->userAgreementLabel->setText("All files successfully downloaded");
  if (this->_dest_file != 0)
    {
      if (!this->_dest_file->exists())
        QMessageBox::warning(this, "Cannot continue !", "The updater is not available");
      else if(system(this->_dest_file->fileName().toStdString().c_str()))
        QMessageBox::warning(this, "Cannot continue !", "The updater is not executable");
      exit(EXIT_FAILURE);
    }
}

void MainWindow::on_cancelInstall_clicked()
{
  // We dont really care
  exit(EXIT_FAILURE);
}

void MainWindow::can_read()
{
  assert(this->_reply != 0);
  assert(this->_dest_file != 0);
  if (this->_reply->bytesAvailable() <= 0)
    return;
  char buf[4096];
  while (this->_reply->bytesAvailable() > 0)
    {
      qint64 read_bytes = this->_reply->read(buf, 4096);
      if (read_bytes > 0)
        {
          if (this->_dest_file->write(buf, read_bytes) != read_bytes)
            {
              // XXX This should not happen
              std::cerr << "ARGHLHLGLHLH\n";
            }
        }
    }
}

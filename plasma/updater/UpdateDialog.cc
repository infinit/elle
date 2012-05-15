
#include <cassert>

#include "UpdateDialog.hh"
#include "ui_UpdateDialog.h"

UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::UpdateDialog)
{
  this->_ui->setupUi(this);
  this->_ui->downloadBar->setValue(0);
  this->_ui->downloadBar->setMinimum(0);
  this->_ui->downloadBar->setMaximum(100000);
  this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::Dialog);
  this->setStyleSheet("background-image: url(http://cdn.sstatic.net/stackoverflow/img/sprites.png?v=5);");
}

UpdateDialog::~UpdateDialog()
{
    delete this->_ui;
    this->_ui = nullptr;
}


void UpdateDialog::setProgress(float percent)
{
  if (percent <= 0) percent = 0;
  if (percent >= 100) percent = 100;
  this->_ui->downloadBar->setValue(percent * (this->_ui->downloadBar->maximum() / 100));
}

void UpdateDialog::setStatus(std::string const& status)
{
  this->_ui->downloadStatus->setText(status.c_str());
}

void UpdateDialog::on_cancelButton_clicked()
{
  Q_EMIT cancelled();
}


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
  this->setWindowFlags(Qt::FramelessWindowHint);
}

UpdateDialog::~UpdateDialog()
{
    delete this->_ui;
    this->_ui = nullptr;
}


void UpdateDialog::setProgress(float percent)
{
  assert(percent >= 0 && "negative percent !");
  assert(percent <= 100 && "Over 100% is reserved for chuck norris progress bars");
  this->_ui->downloadBar->setValue(percent * (this->_ui->downloadBar->maximum() / 100));
}

void UpdateDialog::setStatus(std::string const& status)
{
  this->_ui->downloadStatus->setText(status.c_str());
}

void UpdateDialog::on_cancelButton_clicked()
{
  emit cancelled();
}

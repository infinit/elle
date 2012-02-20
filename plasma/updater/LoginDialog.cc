
#include "LoginDialog.hh"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::GetLoginPassword(std::string& login, std::string& password)
{
  if (this->ui && this->ui->email && this->ui->password)
    {
      login = this->ui->email->text().toStdString();
      password = this->ui->password->text().toStdString();
    }
}

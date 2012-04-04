
#include <iostream>

#include "LoginDialog.hh"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->connect(this, SIGNAL(accepted()), this, SLOT(_OnAccepted()));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::SetErrorMessage(std::string const& error)
{
  this->ui->error->setText(error.c_str());
}

void LoginDialog::_OnAccepted()
{
  if (this->ui && this->ui->email && this->ui->password)
    {
      Q_EMIT doLogin(this->ui->email->text().toStdString(),
                   this->ui->password->text().toStdString());
    }
  else
    {
      std::cout << "Bah goul\n";
    }
}

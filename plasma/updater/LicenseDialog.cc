
#include "LicenseDialog.hh"
#include "ui_LicenseDialog.h"

LicenseDialog::LicenseDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::LicenseDialog)
{
    this->_ui->setupUi(this);
}

LicenseDialog::~LicenseDialog()
{
    delete this->_ui;
    this->_ui = nullptr;
}

void LicenseDialog::on_acceptButton_clicked()
{
  Q_EMIT accepted();
}

void LicenseDialog::on_cancelButton_clicked()
{
  Q_EMIT cancelled();
}

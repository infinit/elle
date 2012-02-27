#ifndef PLASMA_UPDATER_LOGINDIALOG_HH
#define PLASMA_UPDATER_LOGINDIALOG_HH

#include <QDialog>

namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    void GetLoginPassword(std::string& login, std::string& password);
    void SetErrorMessage(std::string const& error);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_HH

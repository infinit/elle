#ifndef PLASMA_UPDATER_LOGINDIALOG_HH
#define PLASMA_UPDATER_LOGINDIALOG_HH

#include <QDialog>

namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::LoginDialog *ui;

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    void SetErrorMessage(std::string const& error);

Q_SIGNALS:
    void doLogin(std::string const& login, std::string const& password);

private Q_SLOTS:
    void _OnAccepted();
};

#endif // LOGINDIALOG_HH

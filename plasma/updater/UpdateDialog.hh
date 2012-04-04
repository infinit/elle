#ifndef UPDATEDIALOG_HH
#define UPDATEDIALOG_HH

#include <QDialog>

namespace Ui {
    class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::UpdateDialog* _ui;

public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

    void setProgress(float percent);
    void setStatus(std::string const& status);

private Q_SLOTS:
    void on_cancelButton_clicked();

Q_SIGNALS:
    void cancelled();
};

#endif // UPDATEDIALOG_HH

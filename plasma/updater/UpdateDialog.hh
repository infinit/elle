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

private slots:
    void on_cancelButton_clicked();

signals:
    void cancelled();
};

#endif // UPDATEDIALOG_HH

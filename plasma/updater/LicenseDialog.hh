#ifndef LICENSEDIALOG_HH
# define LICENSEDIALOG_HH

# include <QDialog>

namespace Ui
{
  class LicenseDialog;
}

class LicenseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LicenseDialog(QWidget *parent = 0);
    ~LicenseDialog();

private:
    Ui::LicenseDialog* _ui;

private slots:
  void on_acceptButton_clicked();
  void on_cancelButton_clicked();

signals:
    void accepted();
    void cancelled();
};

#endif // LICENSEDIALOG_HH

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <cstdlib>
#include <iostream>

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void _onCancelInstall();
    void _onLaunchInstall();
    void _onUserAgree(bool f);

private:
    Ui::MainWindow *_ui;
    bool _userAgreed;
};

#endif // MAINWINDOW_HH

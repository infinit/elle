#ifndef SHELLAPP_HH
# define SHELLAPP_HH

# include <QCoreApplication>

class ReadLineThread;

class ShellApp : public QCoreApplication
{
  Q_OBJECT;

public:
  ShellApp(int argc, char ** argv);

public slots:
  void RunCommand(QString cmd);

protected:
  ReadLineThread * rlt;
};

#endif /* !SHELLAPP_HH */

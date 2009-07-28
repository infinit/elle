#ifndef SHELLAPP_HH
# define SHELLAPP_HH

# include <QCoreApplication>
# include "hole/DHT.hh"

class ReadLineThread;

class ShellApp : public QCoreApplication
{
  Q_OBJECT;

public:
  ShellApp(int argc, char ** argv);

signals:
  void CanReadLine();

public slots:
  void RunCommand(QString cmd);

private:
  void Create(const QStringList & args);
  void Join(const QStringList & args);
  void Port(const QStringList & args);
  void Quit(const QStringList & args);
  void Exit(const QStringList & args);
  void Help(const QStringList & args);

protected:
  ReadLineThread * rlt;
  hole::DHT dht;
};

#endif /* !SHELLAPP_HH */

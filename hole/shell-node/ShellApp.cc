#include <QtDebug>
#include <QStringList>
#include <QTimer>

#include <hole/DHTRequest.hh>

#include "ShellApp.hh"
#include "ReadLineThread.hh"

ShellApp::ShellApp(int ac, char ** av)
  : QCoreApplication(ac, av),
    rlt(new ReadLineThread(this)),
    dht(this)
{
  rlt->start();
  connect(rlt, SIGNAL(LineRead(QString)), this, SLOT(RunCommand(QString)));
  connect(this, SIGNAL(CanReadLine()), rlt, SLOT(ReadLine()));
  emit CanReadLine();
}

void
ShellApp::RunCommand(QString cmd)
{
  static const struct {
    const char * name;
    void (ShellApp::*method)(const QStringList & args);
  } tab[] = {
    { "help", &ShellApp::Help },
    { "create", &ShellApp::Create },
    { "join", &ShellApp::Join },
    { "exit", &ShellApp::Exit },
    { "quit", &ShellApp::Quit },
    { 0, 0 }
  };
  QStringList args = cmd.split(" ", QString::SkipEmptyParts);

  if (args.count() <= 0)
    return;

  for (unsigned i = 0; tab[i].name; i++)
    if (!args[0].compare(tab[i].name, Qt::CaseInsensitive))
    {
      (this->*(tab[i].method))(args);
      goto can_read_line;
    }

  puts("Invalid command");
  /* TODO:
   * put file
   * get $key */
  can_read_line:
  emit CanReadLine();
}

void
ShellApp::Create(const QStringList &)
{
  dht.Create();
}

void
ShellApp::Join(const QStringList & args)
{
  /* check usage */
  if (args.count() != 3)
  {
    puts("usage: join <host> <port>");
    return;
  }

  dht.Join(QHostAddress(args[1]), args[2].toUShort());
}

void
ShellApp::Exit(const QStringList &)
{
  disconnect(rlt, SLOT(ReadLine()));
  rlt->quit();
  rlt->wait();
  QTimer::singleShot(0, this, SLOT(quit()));
}

void
ShellApp::Quit(const QStringList &)
{
  dht.Disconnect();
}

void
ShellApp::Help(const QStringList &)
{
    puts("create\tCreate a network\n"
         "join <host> <port>\tJoin a network\n"
         "port [port]\t\tGet or set the listening port\n"
         "quit\t\t\tQuit the network\n"
         "exit\t\t\tExit the program\n");
}

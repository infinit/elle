#include <QtDebug>
#include <QStringList>

#include "ShellApp.hh"
#include "ReadLineThread.hh"

ShellApp::ShellApp(int ac, char ** av)
  : QCoreApplication(ac, av),
    rlt(new ReadLineThread(this))
{
  rlt->start();
  connect(rlt, SIGNAL(ReadLine(QString)), this, SLOT(RunCommand(QString)));
}

void
ShellApp::RunCommand(QString cmd)
{
  QStringList args = cmd.split(" ", QString::SkipEmptyParts);

  if (args.count() <= 0)
    return;

  if (args[0] == "exit" || args[0] == "quit")
  {
    // do it better
    rlt->terminate();
    quit();
    return;
  }

  /* TODO:
   * create
   * join host port
   * disconnect
   * put file
   * get $key */
}

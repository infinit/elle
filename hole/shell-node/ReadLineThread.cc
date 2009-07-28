#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "ReadLineThread.hh"

ReadLineThread::ReadLineThread(QObject * object)
  : QThread(object)
{
}

void
ReadLineThread::run()
{
  exec();
}

void
ReadLineThread::ReadLine()
{
  char * line = readline("shell-node> ");
  emit LineRead(line);
}

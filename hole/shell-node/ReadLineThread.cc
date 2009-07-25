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
  char * line;

  while ((line = readline("shell-node> ")))
  {
    emit ReadLine(line);
  }
}

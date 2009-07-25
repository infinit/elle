#include "ShellApp.hh"

int main(int argc, char **argv)
{
  ShellApp shell(argc, argv);

  return shell.exec();
}

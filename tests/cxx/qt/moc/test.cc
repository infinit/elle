#include <iostream>

#include "test.hh"

Test::Test()
{}

void
Test::trigger()
{
  emit signal();
}

void
Test::slot()
{
  std::cerr << "slot" << std::endl;
}

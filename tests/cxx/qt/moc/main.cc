#include "test.hh"
#include "main.hh"

int main()
{
  Test t;
  t.connect(&t, SIGNAL(signal()), SLOT(slot()));
  t.trigger();
  return 0;
}


#include <elle/log.hh>

ELLE_LOG_COMPONENT("test_log");

void g()
{
  ELLE_TRACE("Me too!");
}

void f()
{
  ELLE_TRACE("This is usefull!")
  {
    ELLE_TRACE("This is usefull inner");
    g();
  }
}

int main()
{
  ELLE_LOG_COMPONENT("test_log.biet");
  ELLE_LOG("BIET");
  ELLE_TRACE("BIET");
  ELLE_DEBUG("BIET");
  ELLE_DUMP("BIET");
  f();
  return 0;
}

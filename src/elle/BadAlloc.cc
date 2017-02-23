#include <elle/BadAlloc.hh>

namespace elle
{
  BadAlloc::BadAlloc()
    : elle::Error("bad allocation")
  {}
}

#include <nucleus/Exception.hh>

#include <elle/concurrency/Scheduler.hh>

namespace nucleus
{

  Exception::Exception(elle::String const& message):
    reactor::Exception(elle::concurrency::scheduler(),
                       message)
  {
  }

  Exception::~Exception() throw()
  {
  }

}

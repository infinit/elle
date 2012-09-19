#include <elle/standalone/Standalone.hh>
#include <elle/standalone/Log.hh>

#include <elle/standalone/Report.hh>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace standalone
  {

    Status              Standalone::Initialize()
    {
      // initialize the log.
      if (Log::Initialize() == Status::Error)
        escape("unable to initialize the log");

      return Status::Ok;
    }

    Status              Standalone::Clean()
    {
      // clean the log.
      if (Log::Clean() == Status::Error)
        escape("unable to clean the log");

      return Status::Ok;
    }

  }
}

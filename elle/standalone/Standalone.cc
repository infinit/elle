#include <elle/standalone/Standalone.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>
#include <elle/standalone/Morgue.hh>

namespace elle
{
  namespace standalone
  {

    Status              Standalone::Initialize()
    {
      // initialize the report.
      if (Report::Initialize() == Status::Error)
        escape("unable to initialize the report");

      // initialize the log.
      if (Log::Initialize() == Status::Error)
        escape("unable to initialize the log");

      // initialize the morgue.
      if (Morgue::Initialize() == Status::Error)
        escape("unable to initialize the morgue");

      return Status::Ok;
    }

    Status              Standalone::Clean()
    {
      // clean the morgue class.
      if (Morgue::Clean() == Status::Error)
        escape("unable to clean the morgue");

      // clean the log.
      if (Log::Clean() == Status::Error)
        escape("unable to clean the log");

      // clean the report.
      if (Report::Clean() == Status::Error)
        escape("unable to clean the report");

      return Status::Ok;
    }

  }
}

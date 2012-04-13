
#include <elle/standalone/Standalong.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace standalone
  {

    Status              Standalone::Initialize()
    {
      // initialize the report.
      if (Report::Initialize() == StatusError)
        escape("unable to initialize the report");

      // initialize the log.
      if (Log::Initialize() == StatusError)
        escape("unable to initialize the log");

      // initialize the morgue.
      if (Morgue::Initialize() == StatusError)
        escape("unable to initialize the morgue");

      return StatusOk;
    }

    Status              Standalone::Clean()
    {
      // clean the morgue class.
      if (Morgue::Clean() == StatusError)
        escape("unable to clean the morgue");

      // clean the log.
      if (Log::Clean() == StatusError)
        escape("unable to clean the log");

      // clean the report.
      if (Report::Clean() == StatusError)
        escape("unable to clean the report");

      return StatusOk;
    }

  }
}

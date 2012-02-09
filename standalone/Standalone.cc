//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun may  2 11:00:51 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Standalone.hh>

namespace elle
{
  namespace standalone
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the module.
    ///
    Status              Standalone::Initialize()
    {
      ;

      // initialize the report.
      if (Report::Initialize() == StatusError)
        escape("unable to initialize the report");

      // initialize the log.
      if (Log::Initialize() == StatusError)
        escape("unable to initialize the log");

      // initialize the morgue.
      if (Morgue::Initialize() == StatusError)
        escape("unable to initialize the morgue");

      return elle::StatusOk;
    }

    ///
    /// this method cleans the module.
    ///
    Status              Standalone::Clean()
    {
      ;

      // clean the morgue class.
      if (Morgue::Clean() == StatusError)
        escape("unable to clean the morgue");

      // clean the log.
      if (Log::Clean() == StatusError)
        escape("unable to clean the log");

      // clean the report.
      if (Report::Clean() == StatusError)
        escape("unable to clean the report");

      return elle::StatusOk;
    }

  }
}

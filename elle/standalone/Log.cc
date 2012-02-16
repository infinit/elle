//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed jul 27 09:53:07 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Log.hh>
#include <elle/standalone/Region.hh>
#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;

  namespace standalone
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this global variable represent the log for the current program.
    ///
    Log*                Log::Current = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the log system.
    ///
    Status              Log::Initialize()
    {
      // nothing to do.

      return elle::StatusOk;
    }

    ///
    /// this method cleans the log system.
    ///
    Status              Log::Clean()
    {
      // if the log exists, delete it.
      if (Log::Current != NULL)
        delete Log::Current;

      return elle::StatusOk;
    }

    ///
    /// this method sets up the log by allocating a default
    /// log object.
    ///
    Status              Log::Setup(const String&                path)
    {
      // allocate the log for the initial thread/fiber.
      Log::Current = new Log(path);

      return elle::StatusOk;
    }

    ///
    /// this method returns the current instance of the log.
    ///
    /// note that this method returns true or false.
    ///
    Status              Log::Instance(Log*&                     log)
    {
      // verify the log's presence.
      if (Log::Current == NULL)
        return elle::StatusFalse;

      log = Log::Current;

      return elle::StatusTrue;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Log::Log(const String&                                      path)
      : stream_(path.c_str(), std::ios_base::out | std::ios_base::trunc)
    {
    }

    ///
    /// this destructor deletes the stack.
    ///
    Log::~Log()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds a message to the log.
    ///
    Void                Log::Record(const String&               location,
                                    const String&               time,
                                    const String&               message)
    {
      Report*                   report;

      //
      // log the actual message.
      //
      stream_ << message << " (" << location << ") @ " << time << std::endl;

      //
      // log the report, if present.
      //

      // retrieve the report.
      if (Report::Instance(report) == StatusTrue)
        {
          Report::Scoutor       scoutor;

          // go through the report messages.
          for (scoutor = report->container.begin();
               scoutor != report->container.end();
               scoutor++)
            {
              Report::Entry*            entry = *scoutor;

              // build the report.
              stream_ << "  "
                      << entry->message
                      << " ("
                      << entry->location
                      << ") @ "
                      << entry->time
                      << std::endl;
            }

          // flush the report.
          report->Flush();
        }

      // flush the stream
      stream_.flush();
    }

  }
}

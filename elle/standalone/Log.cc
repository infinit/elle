//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Log.cc
//
// created       julien quintard   [wed jul 27 09:53:07 2011]
// updated       julien quintard   [mon aug  1 10:28:16 2011]
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
    Log*		Log::Current = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the log system.
    ///
    Status		Log::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the log system.
    ///
    Status		Log::Clean()
    {
      enter();

      // if the log exists, delete it.
      if (Log::Current != NULL)
	delete Log::Current;

      leave();
    }

    ///
    /// this method sets up the log by allocating a default
    /// log object.
    ///
    Status		Log::Setup(const String&		path)
    {
      enter();

      // allocate the log for the initial thread/fiber.
      Log::Current = new Log(path);

      leave();
    }

    ///
    /// this method returns the current instance of the log.
    ///
    /// note that this method returns true or false.
    ///
    Status		Log::Instance(Log*&			log)
    {
      enter();

      // verify the log's presence.
      if (Log::Current == NULL)
	false();

      log = Log::Current;

      true();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Log::Log(const String&					path)
    {
      // open the file.
      this->fd = ::open(path.c_str(),
			O_WRONLY | O_APPEND | O_CREAT,
			0600);
    }

    ///
    /// this destructor deletes the stack.
    ///
    Log::~Log()
    {
      // close the file.
      ::close(this->fd);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds a message to the log.
    ///
    Void		Log::Record(const String&		location,
				    const String&		time,
				    const String&		message)
    {
      Report*			report;

      //
      // log the actual message.
      //
      {
	std::ostringstream	stream;

	// build the string.
	stream << message << " (" << location << ") @ " << time << std::endl;

	// write it to the log.
	::write(this->fd, stream.str().c_str(), stream.str().length());
      }

      //
      // log the report, if present.
      //
      {
	// retrieve the report.
	if (Report::Instance(report) == StatusTrue)
	  {
	    Report::Scoutor	scoutor;

	    // go through the report messages.
	    for (scoutor = report->container.begin();
		 scoutor != report->container.end();
		 scoutor++)
	      {
		Report::Entry*		entry = *scoutor;
		std::ostringstream	stream;

		// build the report.
		stream << "  "
		       << entry->message
		       << " ("
		       << entry->location
		       << ") @ "
		       << entry->time
		       << std::endl;

		// write it to the log.
		::write(this->fd, stream.str().c_str(), stream.str().length());
	      }

	    // flush the report.
	    report->Flush();
	  }
      }
    }

  }
}

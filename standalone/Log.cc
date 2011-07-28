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
// updated       julien quintard   [wed jul 27 10:17:43 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Log.hh>
#include <elle/standalone/Region.hh>
#include <elle/standalone/Maid.hh>

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
    /// this method initializes the log by allocating a default
    /// log object.
    ///
    Status		Log::Initialize(const Path&		path)
    {
      enter();

      // allocate the log for the initial thread/fiber.
      Log::Current = new Log(path);

      leave();
    }

    ///
    /// this method cleans the log system.
    ///
    Status		Log::Clean()
    {
      enter();

      // delete the log.
      delete Log::Current;

      // reset the pointer.
      Log::Current = NULL;

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
    Log::Log(const Path&					path):
      path(path)
    {
      // open the file.
      this->fd = ::open(path.string.c_str(),
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
      // XXX
    }

  }
}

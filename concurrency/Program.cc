//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Program.cc
//
// created       julien quintard   [mon mar 15 20:40:02 2010]
// updated       julien quintard   [wed mar 31 21:34:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Program.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- globals ---------------------------------------------------------
//

    ///
    /// this variable represents the program.
    ///
    Program*			program = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the program.
    ///
    Status		Program::Initialize()
    {
      enter();

      // allocate a new program.
      program = new Program;

      leave();
    }

    ///
    /// this method cleans the program.
    ///
    Status		Program::Clean()
    {
      enter();

      // delete the program.
      if (program != NULL)
	delete program;

      leave();
    }

    ///
    /// this method sets up the program for startup.
    ///
    Status		Program::Setup(const Natural32	argc,
					   const Character*	argv[],
					   Callback<>*		prolog,
					   Callback<>*		epilog)
    {
      enter();

      // set the arguments.
      program->argc = argc;
      program->argv = argv;

      // allocate the QT program.
      program->core = new ::QCoreApplication((int&)program->argc,
					     (char**)program->argv);

      // set the prolog/epilog callbacks.
      program->prolog = prolog;
      program->epilog = epilog;

      leave();
    }

    ///
    /// this method stops the program.
    ///
    Status		Program::Exit(Status		status)
    {
      enter();

      // lock in writing.
      program->accord.Lock(ModeWrite);
      {
	// set the status.
	program->status = status;

	// set the exit state.
	program->state = Program::StateStopped;
      }
      program->accord.Unlock();

      leave();
    }

    ///
    /// this method processes events.
    ///
    Status		Program::Launch()
    {
      enter();

      // check the program.
      if ((program == NULL) || (program->core == NULL))
	escape("unable to process events since the program has not "
	       "been set up");

      // process the events.
      while (true)
	{
	  // lock in reading.
	  program->accord.Lock(ModeRead);
	  {
	    // check if the program must be stopped.
	    if (program->state == Program::StateStopped)
	      {
		// release the objects.
		release();

		return (program->status);
	      }
	  }
	  program->accord.Unlock();

	  // call the prolog, if provided.
	  if (program->prolog != NULL)
	    {
	      if (program->prolog->Trigger() == StatusError)
		escape("an error occured in the prolog");
	    }

	  // process the QT events.
	  program->core->processEvents();

	  // call the epilog, if provided.
	  if (program->epilog != NULL)
	    {
	      if (program->epilog->Trigger() == StatusError)
		escape("an error occured in the epilog");
	    }

	  // lock in reading.
	  program->accord.Lock(ModeRead);
	  {
	    // if there are no events left to process, sleep a bit in order
	    // to prevent using 100% of the CPU.
	    if (program->core->hasPendingEvents() == false)
	      {
		program->accord.Unlock();

		// sleep.
		::usleep(10000);
	      }
	    else
	      program->accord.Unlock();
	  }
	}

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Program::Program():
      core(NULL),
      state(Program::StateUnknown),
      status(StatusUnknown),
      prolog(NULL),
      epilog(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Program::~Program()
    {
      // releae the core.
      if (this->core != NULL)
	delete this->core;
    }

  }
}

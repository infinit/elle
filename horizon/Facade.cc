//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien.quintard   [thu dec 22 16:44:01 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <facade/Facade.hh>

namespace facade
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this variable holds the mountpoint on which the facade is operating.
  ///
  elle::String			Facade::Mountpoint;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the facade.
  ///
  elle::Status		Facade::Initialize()
  {
    enter();

    // XXX everything must change!
    if (Infinit::Parser->Test("Mountpoint") == elle::StatusFalse)
      {
	char		command[256];

	Facade::Mountpoint =
	  elle::System::Path::Home + "/local/mnt/infinit/personal";

	printf("Infinit is about to be mounted at '%s'\n",
	       Facade::Mountpoint.c_str());

	sprintf(command, "mkdir -p %s",
		Facade::Mountpoint.c_str());
	system(command);
      }
    else
      {
	// retrieve the mount point.
	if (Infinit::Parser->Value("Mountpoint",
				   Facade::Mountpoint) == elle::StatusError)
	  {
	    // display the usage.
	    Infinit::Parser->Usage();

	    escape("unable to retrieve the mount point");
	  }
      }

#if defined(INFINIT_UNIX)
    // initialize the UNIX implementation.
    if (unix::UNIX::Initialize() == elle::StatusError)
      escape("unable to initialize the UNIX implementation");
#elif defined(INFINIT_WIN32)
    // XXX to do!
#endif

    leave();
  }

  ///
  /// this method cleans the facade.
  ///
  elle::Status		Facade::Clean()
  {
    enter();

#if defined(INFINIT_UNIX)
    // clean the UNIX implementation.
    if (unix::UNIX::Clean() == elle::StatusError)
      escape("unable to clean the UNIX implementation");
#elif defined(INFINIT_WIN32)
    // XXX todo: windows!
#endif

    leave();
  }

  ///
  /// this method sets up the facade-specific options.
  ///
  elle::Status		Facade::Options()
  {
    enter();

    // register the option.
    if (Infinit::Parser->Register(
	  "Mountpoint",
	  'm',
	  "mountpoint",
	  "specifies the mount point",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    leave();
  }

}

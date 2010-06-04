//
// ---------- header ----------------------------------------------------------
//
// project       8setup
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8setup/Setup.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [mon may 10 17:33:11 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8setup/Setup.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8setup";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method empties a directory.
  ///
  elle::Status		Setup::Empty(const elle::String&	path)
  {
    ::DIR*		directory;
    struct ::dirent*	entry;

    enter();

    // open the directory.
    if ((directory = ::opendir(path.c_str())) == NULL)
      escape("unable to open the directory");

    // go through the entries.
    while ((entry = ::readdir(directory)) != NULL)
      {
	// ignore the . and ..
	if ((::strcmp(entry->d_name, ".") == 0) ||
	    (::strcmp(entry->d_name, "..") == 0))
	  continue;

	// if the entry is a directory.
	if (entry->d_type == DT_DIR)
	  {
	    elle::String	target =
	      path +
	      elle::System::Path::Separator +
	      elle::String(entry->d_name);

	    // empty it as well.
	    if (Setup::Empty(target) == elle::StatusError)
	      escape("unable to empty a subdirectory");

	    // remove the directory.
	    if (::rmdir(target.c_str()) == -1)
	      escape("unable to remove the subdirectory");
	  }
	else
	  {
	    elle::String	target =
	      path +
	      elle::System::Path::Separator +
	      elle::String(entry->d_name);

	    // otherwise, just remove the file.
	    if (::unlink(target.c_str()) == -1)
	      escape("unable to remove the file");
	  }
      }

    // close the directory.
    ::closedir(directory);

    leave();
  }

  ///
  /// this method initializes the user's environment.
  ///
  elle::Status		Setup::Initialize()
  {
    enter();

    //
    // empty the infinit directory.
    //
    {
      // empty the directory.
      Setup::Empty(lune::Lune::Home);
    }

    //
    // infinit/
    //
    {
      // create the user's Infinit home directory.
      if (::mkdir(lune::Lune::Home.c_str(), 0700) == -1)
	escape("unable to create the infinit home directory");
    }

    //
    // infinit/users/
    //
    {
      // create the keys directory.
      if (::mkdir(lune::Lune::Users.c_str(), 0700) == -1)
	escape("unable to create the users directory");
    }

    //
    // infinit/universes/
    //
    {
      // create the universes directory.
      if (::mkdir(lune::Lune::Universes.c_str(), 0700) == -1)
	escape("unable to create the universes directory");
    }

    //
    // infinit/map
    //
    {
      lune::Associat	associat;

      // store an initial, empty, associat.
      if (associat.Store() == elle::StatusError)
	escape("unable to store the associat");
    }

    leave();
  }

  ///
  /// this method cleans the user's environment.
  ///
  elle::Status		Setup::Clean()
  {
    enter();

    // empty the home.
    if (Setup::Empty(lune::Lune::Home) == elle::StatusError)
      escape("unable to empty the home directory");

    // destroy the user's Infinit home directory.
    if (::rmdir(lune::Lune::Home.c_str()) == -1)
      escape("unable to remove the infinit home directory");

    // display a message.
    std::cout << "The infinit environment has been cleaned successfully!"
	      << std::endl;

    leave();
  }

  ///
  /// this method checks if the user's environment is valid i.e all the
  /// required directories exist, with the correct permissions.
  ///
  elle::Status		Setup::Check()
  {
    struct ::stat	stat;

    enter();

    /*
    //
    // infinit/
    //
    {
      // test the home directory.
      if (::stat(Lune::Home.c_str(), &stat) == -1)
	escape("the home infinit directory does not seem to exist");

      // test the type.
      if (S_ISDIR(stat.st_mode) == 0)
	escape("the path to the infinit environment is not a directory");

      // test the permissions.
      if ((stat.st_mode & 0777) != 0700)
	escape("the home infinit directory should have the following "
	       "permissions: 0700");

      //
      // infinit/phrase
      //
      {
	// test the phrase file.
	if (::stat(Lune::Phrase.c_str(), &stat) == 0)
	  {
	    // test the type.
	    if (S_ISREG(stat.st_mode) == 0)
	      escape("the path to the phrase is not a file");

	    // test the permissions.
	    if ((stat.st_mode & 0777) != 0600)
	      escape("the phrase file should have the following "
		     "permissions: 0600");
	  }
      }

      //
      // infinit/authority
      //
      {
	// test the authority file.
	if (::stat(lune::Lune::Authority.c_str(), &stat) == 0)
	  {
	    // test the type.
	    if (S_ISREG(stat.st_mode) == 0)
	      escape("the path to the authority is not a file");

	    // test the permissions.
	    if ((stat.st_mode & 0777) != 0600)
	      escape("the authority file should have the following "
		     "permissions: 0600");
	  }
      }
    }

    //
    // infinit/universes/
    //
    {
      // test the universes directory.
      if (::stat(lune::Lune::Universes.c_str(), &stat) == -1)
	escape("the universes directory does not seem to exist");

      // test the type.
      if (S_ISDIR(stat.st_mode) == 0)
	escape("the path to the universes is not a directory");

      // test the permissions.
      if ((stat.st_mode & 0777) != 0700)
	escape("the universes directory should have the following "
	       "permissions: 0700");
    }

    //
    // infinit/users/
    //
    {
      // test the keys directory.
      if (::stat(lune::Lune::Users.c_str(), &stat) == -1)
	escape("the userss directory does not seem to exist");

      // test the type.
      if (S_ISDIR(stat.st_mode) == 0)
	escape("the path to the users is not a directory");

      // test the permissions.
      if ((stat.st_mode & 0777) != 0700)
	escape("the users directory should have the following "
	       "permissions: 0700");
    }

    // display a message.
    std::cout << "The infinit environment seems to be valid!"
	      << std::endl;
    */
    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
  {
    elle::Parser*	parser;
    Setup::Operation	operation;
    elle::Character	option;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // initialize the Etoile.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Setup::OperationUnknown;

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('i',
			 "initialize",
			 "initialize the user's infinit environment",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "clean",
			 "clean the user's infinit environment",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('k',
			 "check",
			 "check if the user's infinit environment is valid",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == elle::StatusTrue)
      {
	switch (option)
	  {
	  case 'h':
	    {
	      // display the usage.
	      parser->Usage();

	      // quit.
	      leave();
	    }
	  case 'i':
	    {
	      // check if the operation has already been set up.
	      if (operation != Setup::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the initialize operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Setup::OperationInitialize;

	      break;
	    }
	  case 'c':
	    {
	      // check if the operation has already been set up.
	      if (operation != Setup::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the clean operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Setup::OperationClean;

	      break;
	    }
	  case 'k':
	    {
	      // check if the operation has already been set up.
	      if (operation != Setup::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the check operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Setup::OperationCheck;

	      break;
	    }
	  case '?':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("unknown option");
	    }
	  case ':':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("missing argument");
	    }
	  default:
	    {
	      escape("an error occured while parsing the options");
	    }
	  }
      }

    // trigger the operation.
    switch (operation)
      {
      case Setup::OperationInitialize:
	{
	  // initialize the environment.
	  if (Setup::Initialize() == elle::StatusError)
	    escape("unable to initialize the environment");

	  break;
	}
      case Setup::OperationClean:
	{
	  // clean the environment.
	  if (Setup::Clean() == elle::StatusError)
	    escape("unable to clean the environment");

	  break;
	}
      case Setup::OperationCheck:
	{
	  // check the environment.
	  if (Setup::Check() == elle::StatusError)
	    escape("unable to check the environment");

	  break;
	}
      case Setup::OperationUnknown:
      default:
	{
	  // display the usage.
	  parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

    // delete the parser.
    delete parser;

    // clean the Etoile.
    if (etoile::Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

    // clean Lune.
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean Lune");

    // clean Elle.
    if (elle::Elle::Clean() == elle::StatusError)
      escape("unable to clean Elle");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int			main(int				argc,
                             char**				argv)
{
  application::Main(argc, argv);

  expose();

  return (0);
}

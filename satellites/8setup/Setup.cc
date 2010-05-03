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
// updated       julien quintard   [sat may  1 20:37:21 2010]
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
  const Character	Component[] = "8setup";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method empties a directory.
  ///
  Status		Setup::Empty(const String&		path)
  {
    DIR*		directory;
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
	    String	target =
	      path + System::Path::Separator + String(entry->d_name);

	    // empty it as well.
	    if (Setup::Empty(target) == StatusError)
	      escape("unable to empty a subdirectory");

	    // remove the directory.
	    if (::rmdir(target.c_str()) == -1)
	      escape("unable to remove the subdirectory");
	  }
	else
	  {
	    String	target =
	      path + System::Path::Separator + String(entry->d_name);

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
  Status		Setup::Initialize()
  {
    enter();

    //
    // infinit/
    //
    {
      // create the user's Infinit home directory.
      if (::mkdir(Lune::Home.c_str(), 0700) == -1)
	escape("unable to create the infinit home directory");
    }

    //
    // infinit/keys/
    //
    {
      // create the keys directory.
      if (::mkdir(Lune::Keys.c_str(), 0700) == -1)
	escape("unable to create the keys directory");
    }

    //
    // infinit/universes/
    //
    {
      // create the universes directory.
      if (::mkdir(Lune::Universes.c_str(), 0700) == -1)
	escape("unable to create the universes directory");
    }

    //
    // infinit/reserve/
    //
    {
      // create the reserve directory.
      if (::mkdir(Lune::Reserve.c_str(), 0700) == -1)
	escape("unable to create the reserve directory");
    }

    //
    // infinit/hole
    //
    {
      // create the hole directory.
      if (::mkdir(Lune::Hole.c_str(), 0700) == -1)
	escape("unable to create the hole directory");
    }

    // display a message.
    std::cout << "The infinit environment has been initialized successfully!"
	      << std::endl;

    leave();
  }

  ///
  /// this method cleans the user's environment.
  ///
  Status		Setup::Clean()
  {
    enter();

    //
    // infinit/hole/
    //
    {
      // empty the hole.
      if (Setup::Empty(Lune::Hole) == StatusError)
	escape("unable to empty the hole directory");

      // destroy the hole directory.
      if (::rmdir(Lune::Hole.c_str()) == -1)
	escape("unable to remove the hole directory");
    }

    //
    // infinit/reserve/
    //
    {
      // empty the reserve.
      if (Setup::Empty(Lune::Reserve) == StatusError)
	escape("unable to empty the reserve directory");

      // destroy the reserve directory.
      if (::rmdir(Lune::Reserve.c_str()) == -1)
	escape("unable to remove the reserve directory");
    }

    //
    // infinit/keys/
    //
    {
      // empty the keys.
      if (Setup::Empty(Lune::Keys) == StatusError)
	escape("unable to empty the keys directory");

      // destroy the keys directory.
      if (::rmdir(Lune::Keys.c_str()) == -1)
	escape("unable to remove the keys directory");
    }

    //
    // infinit/universes/
    //
    {
      // empty the universes.
      if (Setup::Empty(Lune::Universes) == StatusError)
	escape("unable to empty the universes directory");

      // destroy the universes directory.
      if (::rmdir(Lune::Universes.c_str()) == -1)
	escape("unable to remove the universes directory");
    }

    //
    // infinit/
    //
    {
      // empty the home.
      if (Setup::Empty(Lune::Home) == StatusError)
	escape("unable to empty the home directory");

      // destroy the user's Infinit home directory.
      if (::rmdir(Lune::Home.c_str()) == -1)
	escape("unable to remove the infinit home directory");
    }

    // display a message.
    std::cout << "The infinit environment has been cleaned successfully!"
	      << std::endl;

    leave();
  }

  ///
  /// this method checks if the user's environment is valid i.e all the
  /// required directories exist, with the correct permissions.
  ///
  Status		Setup::Check()
  {
    struct ::stat	stat;

    enter();

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
      // infinit/identity
      //
      {
	// test the identity file.
	if (::stat(Lune::Identity.c_str(), &stat) == 0)
	  {
	    // test the type.
	    if (S_ISREG(stat.st_mode) == 0)
	      escape("the path to the identity is not a file");

	    // test the permissions.
	    if ((stat.st_mode & 0777) != 0600)
	      escape("the identity file should have the following "
		     "permissions: 0600");
	  }
      }
    }

    //
    // infinit/keys/
    //
    {
      // test the keys directory.
      if (::stat(Lune::Keys.c_str(), &stat) == -1)
	escape("the keys directory does not seem to exist");

      // test the type.
      if (S_ISDIR(stat.st_mode) == 0)
	escape("the path to the keys is not a directory");

      // test the permissions.
      if ((stat.st_mode & 0777) != 0700)
	escape("the keys directory should have the following "
	       "permissions: 0700");

      //
      // infinit/keys/*
      //
      {
	DIR*			directory;
	struct ::dirent*	entry;

	// open the directory.
	if ((directory = ::opendir(Lune::Keys.c_str())) ==
	    NULL)
	  escape("unable to open the keys directory");

	// go through the entries.
	while ((entry = ::readdir(directory)) != NULL)
	  {
	    // ignore the . and ..
	    if ((::strcmp(entry->d_name, ".") == 0) ||
		(::strcmp(entry->d_name, "..") == 0))
	      continue;

	    // if the entry is a file.
	    if (entry->d_type == DT_REG)
	      {
		String	target =
		  Lune::Keys +
		  System::Path::Separator +
		  String(entry->d_name);

		// test the key file.
		if (::stat(target.c_str(), &stat) == -1)
		  escape("the key file does not seem to exist");

		// test the permissions.
		if ((stat.st_mode & 0777) != 0400)
		  escape("the key files should have the following "
			 "permissions: 0400");
	      }
	  }

	// close the directory.
	::closedir(directory);
      }
    }

    //
    // infinit/universes/
    //
    {
      // test the universes directory.
      if (::stat(Lune::Universes.c_str(), &stat) == -1)
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
    // infinit/reserve/
    //
    {
      // test the reserve directory.
      if (::stat(Lune::Reserve.c_str(), &stat) == -1)
	escape("the reserve directory does not seem to exist");

      // test the type.
      if (S_ISDIR(stat.st_mode) == 0)
	escape("the path to the reserve is not a directory");

      // test the permissions.
      if ((stat.st_mode & 0777) != 0700)
	escape("the reserve directory should have the following "
	       "permissions: 0700");
    }

    //
    // infinit/hole/
    //
    {
      // test the hole directory.
      if (::stat(Lune::Hole.c_str(), &stat) == -1)
	escape("the hole directory does not seem to exist");

      // test the type.
      if (S_ISDIR(stat.st_mode) == 0)
	escape("the path to the hole is not a directory");

      // test the permissions.
      if ((stat.st_mode & 0777) != 0700)
	escape("the hole directory should have the following "
	       "permissions: 0700");
    }

    // display a message.
    std::cout << "The infinit environment seems to be valid!"
	      << std::endl;

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    Parser*		parser;
    Setup::Operation	operation;
    Character		option;

    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (Program::Setup() == StatusError)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (Lune::Initialize() == StatusError)
      escape("unable to initialize Lune");

    // initialize the Etoile.
    if (Etoile::Initialize() == StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Setup::OperationUnknown;

    // allocate a new parser.
    parser = new Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('i',
			 "initialize",
			 "initialize the user's infinit environment",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "clean",
			 "clean the user's infinit environment",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('k',
			 "check",
			 "check if the user's infinit environment is valid",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == StatusTrue)
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
	  if (Setup::Initialize() == StatusError)
	    escape("unable to initialize the environment");

	  break;
	}
      case Setup::OperationClean:
	{
	  // clean the environment.
	  if (Setup::Clean() == StatusError)
	    escape("unable to clean the environment");

	  break;
	}
      case Setup::OperationCheck:
	{
	  // check the environment.
	  if (Setup::Check() == StatusError)
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
    if (Etoile::Clean() == StatusError)
      escape("unable to clean Etoile");

    // clean Lune.
    if (Lune::Clean() == StatusError)
      escape("unable to clean Lune");

    // clean Elle.
    if (Elle::Clean() == StatusError)
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

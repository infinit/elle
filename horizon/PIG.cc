//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/PIG.cc
//
// created       julien quintard   [tue may 31 10:31:35 2011]
// updated       julien quintard   [wed aug  3 20:26:07 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/PIG.hh>

#include <elle/Elle.hh>
#include <agent/Agent.hh>

namespace pig
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this variable contains the UID of the 'somebody' user, user which
  /// is used whenever the system cannot map the Infinit user on a local
  /// user.
  ///
  uid_t					PIG::Somebody::UID;

  ///
  /// this variable contains the GID of the 'somebody' group.
  ///
  gid_t					PIG::Somebody::GID;

  ///
  /// this varaible contains the mappings between local user/group
  /// identities and Infinit identities.
  ///
  lune::Dictionary			PIG::Dictionary;

  ///
  /// this variable contains the diary which replays the PIG events.
  ///
  pig::diary::Diary			PIG::Diary;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes PIG.
  ///
  elle::Status		PIG::Initialize()
  {
    elle::String	mountpoint;

    enter();

    //
    // initialize the 'somebody' entity.
    //
    {
      struct ::passwd*	passwd;

      // retrieve the passwd structure related to the user 'somebody'.
      if ((passwd = ::getpwnam("somebody")) == NULL)
	escape("it seems that the user 'somebody' does not exist");

      // set the uid and gid.
      PIG::Somebody::UID = passwd->pw_uid;
      PIG::Somebody::GID = passwd->pw_gid;
    }

    //
    // load the user/group maps which will be used to translate Infinit
    // user/group identifiers into local identifiers.
    //
    {
      // load the dictionary file.
      if (PIG::Dictionary.Load(agent::Agent::Identity.name) ==
	  elle::StatusError)
	escape("unable to load the dictionary");
    }

    //
    // initialize FUSE.
    //
    {
      if (FUSE::Initialize() == elle::StatusError)
	escape("unable to initialize FUSE");
    }

    //
    // initialize the diary replaying process, if necessary.
    //
    if (Infinit::Parser->Test("Replay") == elle::StatusTrue)
      {
	elle::String		string;
	elle::Path		path;
	elle::Natural32		from;
	elle::Natural32		to;

	// retrieve the path.
	if (Infinit::Parser->Value("Replay", string) == elle::StatusError)
	  {
	    // display the usage.
	    Infinit::Parser->Usage();

	    escape("unable to retrieve the diary path");
	  }

	// create the path.
	if (path.Create(string) == elle::StatusError)
	  escape("unable to create the path");

	// load the diary.
	if (PIG::Diary.Load(path) == elle::StatusError)
	  escape("unable to load the diary");

	// set up the diary.
	if (PIG::Diary.Setup(FUSE::Operations) == elle::StatusError)
	  escape("unable to set up the diary");

	// initialize the indexes.
	from = 0;
	to = elle::Variable::Maximum(to);

	// retrieve the from.
	if ((Infinit::Parser->Test("From") == elle::StatusTrue) &&
	    (Infinit::Parser->Value(
				    "From",
				    from) == elle::StatusError))
	  {
	    // display the usage.
	    Infinit::Parser->Usage();

	    escape("unable to retrieve the from value");
	  }

	// retrieve the to.
	if ((Infinit::Parser->Test("To") == elle::StatusTrue) &&
	    (Infinit::Parser->Value(
				    "To",
				    to) == elle::StatusError))
	  {
	    // display the usage.
	    Infinit::Parser->Usage();

	    escape("unable to retrieve the to value");
	  }

	// replay the diary.
	if (PIG::Diary.Replay(from, to) == elle::StatusError)
	  escape("unable to replay the diary");
      }
    else
      {
	//
	// otherwise, the replaying has not been activated, hence set up FUSE.
	//
	// retrieve the mount point.
	if (Infinit::Parser->Value("Mountpoint",
				   mountpoint) == elle::StatusError)
	  {
	    // display the usage.
	    Infinit::Parser->Usage();

	    escape("unable to retrieve the mount point");
	  }

	if (FUSE::Setup(mountpoint) == elle::StatusError)
	  escape("unable to set up FUSE");
      }

    leave();
  }

  ///
  /// this method cleans PIG.
  ///
  elle::Status		PIG::Clean()
  {
    enter();

    // clean FUSE.
    if (FUSE::Clean() == elle::StatusError)
      escape("unable to clean FUSE");

    leave();
  }

  ///
  /// this method sets up the pig-specific options.
  ///
  elle::Status		PIG::Options()
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

    // register the option.
    if (Infinit::Parser->Register(
	  "Replay",
	  'y',
	  "replay",
	  "activates the replay of the recorded events",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "From",
	  'f',
	  "from",
	  "specify the number of the first operation to be triggered from "
	  "the diary",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "To",
	  't',
	  "to",
	  "specify the number of the last operation to be triggered from "
	  "the diary",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    leave();
  }

}

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
// updated       julien quintard   [mon jun 27 08:41:09 2011]
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
  /// this variable contains the diary which either records the PIG
  /// events or replays them.
  ///
  pig::Diary				PIG::Diary;

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
    // handle the parser.
    //
    {
      // retrieve the mount point.
      if (Infinit::Parser->Value("Mountpoint",
				 mountpoint) == elle::StatusError)
	escape("unable to retrieve the mount point");

      // check the mutually exclusive options.
      if ((Infinit::Parser->Test("Record") == elle::StatusTrue) &&
	  (Infinit::Parser->Test("Replay") == elle::StatusTrue))
	{
	  // display the usage.
	  Infinit::Parser->Usage();

	  escape("the record and replay options are mutually exclusive");
	}

      // test the option.
      if (Infinit::Parser->Test("Record") == elle::StatusTrue)
	{
	  elle::String		path;

	  // retrieve the path.
	  if (Infinit::Parser->Value("Record", path) == elle::StatusError)
	    escape("unable to retrieve the diary path");

	  // create the diary.
	  if (PIG::Diary.Create(Diary::ModeRecord, path) == elle::StatusError)
	    escape("unable to create the diary");
	}

      // test the option.
      if (Infinit::Parser->Test("Replay") == elle::StatusTrue)
	{
	  elle::String		path;

	  // retrieve the path.
	  if (Infinit::Parser->Value("Replay", path) == elle::StatusError)
	    escape("unable to retrieve the diary path");

	  // create the diary.
	  if (PIG::Diary.Create(Diary::ModeRecord, path) == elle::StatusError)
	    escape("unable to create the diary");
	}
    }

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
      if (FUSE::Initialize(mountpoint) == elle::StatusError)
	escape("unable to initialize FUSE");
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
	  elle::Parser::FormatRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
	  "Record",
	  'c',
	  "record",
	  "activates the event recording feature",
	  elle::Parser::FormatRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
	  "Replay",
	  'y',
	  "replay",
	  "activates the replay of the recorded events",
	  elle::Parser::FormatRequired) == elle::StatusError)
      escape("unable to register the option");

    leave();
  }

}

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
// updated       julien quintard   [fri jun 17 17:10:21 2011]
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
  /// the subject representing the current user.
  ///
  nucleus::Subject*			PIG::Subject;

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

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes PIG.
  ///
  elle::Status		PIG::Initialize(const elle::String&	mountpoint)
  {
    enter();

    //
    // create a subject representing the current user.
    //
    {
      // allocate a new subject.
      PIG::Subject = new nucleus::Subject;

      // create the subject.
      if (PIG::Subject->Create(agent::Agent::Identity.pair.K) ==
	  elle::StatusError)
	escape("unable to create the user's subject");
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

    //
    // destroy the subject.
    //
    {
      // delete the subject instance.
      if (PIG::Subject != NULL)
	delete PIG::Subject;
    }

    //
    // clean FUSE.
    //
    {
      if (FUSE::Clean() == elle::StatusError)
	escape("unable to clean FUSE");
    }

    leave();
  }

}

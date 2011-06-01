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
// updated       julien quintard   [wed jun  1 10:01:45 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/PIG.hh>

#include <elle/Elle.hh>

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
  elle::Status		PIG::Initialize(const elle::String&	user,
					const elle::String&	network,
					const elle::String&	mountpoint)
  {
    lune::Authority	authority;
    lune::Identity	identity;
    lune::Phrase	phrase;

    enter();

    //
    // load the authority.
    //
    {
      elle::PublicKey	K;

      // restore the authority's public key.
      if (K.Restore(Infinit::Authority) == elle::StatusError)
	escape("unable to restore the authority's public key");

      // create the authority based on the hard-coded public key.
      if (authority.Create(K) == elle::StatusError)
	escape("unable to create the authority");
    }

    //
    // load the user identity.
    //
    {
      elle::String	prompt;
      elle::String	pass;

      // does the identity exist.
      if (identity.Exist(user) == elle::StatusFalse)
	escape("the user identity does not seem to exist");

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + user + "': ";
      pass = elle::String(::getpass(prompt.c_str()));

      // load the identity.
      if (identity.Load(user) == elle::StatusError)
	escape("unable to load the identity");

      // verify the identity.
      if (identity.Validate(authority) != elle::StatusTrue)
	escape("the identity seems to be invalid");

      // decrypt the identity.
      if (identity.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the identity");
    }

    //
    // create a subject representing the current user.
    //
    {
      // allocate a new subject.
      PIG::Subject = new nucleus::Subject;

      // create the subject.
      if (PIG::Subject->Create(identity.pair.K) == elle::StatusError)
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
      if (PIG::Dictionary.Load(user) == elle::StatusError)
	escape("unable to load the dictionary");
    }

    //
    // initialize FUSE.
    //
    {
      if (FUSE::Initialize(user, network, mountpoint) == elle::StatusError)
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

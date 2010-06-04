//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Chain.cc
//
// created       julien quintard   [mon may 24 20:55:49 2010]
// updated       julien quintard   [fri may 28 18:07:29 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Chain.hh>
#include <lune/Lune.hh>
#include <lune/Authority.hh>

#include <Infinit.hh>

namespace lune
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// destructor.
  ///
  Chain::~Chain()
  {
    Chain::Scoutor	scoutor;

    // go through the container.
    for (scoutor = this->container.begin();
	 scoutor != this->container.end();
	 scoutor++)
      {
	// delete the passport.
	delete scoutor->second;
      }

    // clear the container.
    this->container.clear();
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method loads the passports associated with the user.
  ///
  elle::Status		Chain::Load(const lune::Identity&	identity)
  {
    /* XXX
    std::pair<Chain::Iterator, elle::Boolean>	result;
    elle::String				path =
      lune::Lune::Users + elle::System::Path::Separator +
      identity.name + elle::System::Path::Separator +
      lune::Lune::Passports;
    ::DIR*					directory;
    struct ::dirent*				entry;
    elle::PublicKey				K;
    lune::Authority				authority;

    enter();

    // restore the authority's public key.
    if (K.Restore(Infinit::Authority) == elle::StatusError)
      escape("unable to restore the authority's public key");

    // create the authority based on the hard-coded public key.
    if (authority.Create(K) == elle::StatusError)
      escape("unable to create the authority");

    // open the user's passports directory.
    if ((directory = ::opendir(path.c_str())) == NULL)
      escape(::strerror(errno));

    // go through the directory entries i.e the passports.
    while ((entry = ::readdir(directory)) != NULL)
      {
	elle::String		file(entry->d_name);
	elle::String		universe;
	lune::Passport*		passport;

	// ignore the . and .. entries.
	if ((file == elle::String(".")) ||
	    (file == elle::String("..")))
	  continue;

	enter(instance(passport));

	// extract the universe name by removing the extension.
	universe = file.substr(0,
			       file.length() -
			       lune::Passport::Extension.length());

	// allocate a new passport.
	passport = new lune::Passport;

	// load the passport.
	if (passport->Load(identity.name, universe) == elle::StatusError)
	  escape("unable to load the passport");

	// validate the passport.
	if (passport->Validate(authority) == elle::StatusError)
	  escape("unable to validate the passport");

	// decrypt the passport.
	if (passport->Decrypt(identity) == elle::StatusError)
	  escape("unable to decrypt the passport");

	// add the passport to the container.
	result = this->container.insert(Chain::Value(universe, passport));

	// check if the insertion was successful.
	if (result.second == false)
	  escape("unable to insert the passport in the container");

	// waive.
	waive(passport);

	// release.
	release();
      }

    leave();
    */
  }

  ///
  /// this method returns the passport for the given universe.
  ///
  elle::Status		Chain::Lookup(const elle::String&	universe,
				      lune::Passport*&		passport)
  {
    Chain::Iterator	iterator;

    enter();

    // try to find the passport in the container.
    if ((iterator = this->container.find(universe)) == this->container.end())
      escape("unable to locate the passport for the given universe");

    // return the passport.
    passport = iterator->second;

    leave();
  }

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a chain.
  ///
  elle::Status		Chain::Dump(const elle::Natural32	margin) const
  {
    Chain::Scoutor	scoutor;
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Chain]" << std::endl;

    // go through the container.
    for (scoutor = this->container.begin();
	 scoutor != this->container.end();
	 scoutor++)
      {
	// dump the passport.
	if (scoutor->second->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the passport");
      }

    leave();
  }

}

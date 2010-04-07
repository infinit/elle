//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/State.cc
//
// created       julien quintard   [wed mar 31 16:21:17 2010]
// updated       julien quintard   [tue apr  6 12:26:37 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/State.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates the state according to the given object.
    ///
    Status		State::Create(const kernel::Object&	object)
    {
      enter();

      // set the genre.
      this->genre = object.meta.genre;

      // set the stamps.
      this->stamps.creation = object.meta.stamp;
      this->stamps.modification = object.data.stamp;

      // set the size
      this->size = object.data.size;

      // set the owner.
      this->Ks.owner = object.owner.K;

      // set the author depending on the mode.
      switch (object.author.role)
	{
	case kernel::RoleOwner:
	  {
	    this->Ks.author = object.owner.K;

	    break;
	  }
	default:
	  {
	    // XXX
	    printf("UNIMPLEMENTED\n");	
	  }
	}

      // set the versions.
      this->versions.data = object.data.version;
      this->versions.meta = object.meta.version;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the state object.
    ///
    Status		State::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[State]" << std::endl;

      // dump the genre.
      std::cout << alignment << Dumpable::Shift << "[Genre] "
		<< this->genre << std::endl;

      //
      // dump the stamps.
      //
      {
	std::cout << alignment << Dumpable::Shift << "[Stamps]" << std::endl;

	// dump the creation time.
	std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		  << "[Creation]" << std::endl;

	if (this->stamps.creation.Dump(margin + 6) == StatusError)
	  escape("unable to dump the creation time");

	// dump the modification time.
	std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		  << "[Modification]" << std::endl;

	if (this->stamps.modification.Dump(margin + 6) == StatusError)
	  escape("unable to dump the modification time");
      }

      // dump the size.
      std::cout << alignment << Dumpable::Shift << "[Size] "
		<< std::dec << this->size << std::endl;

      //
      // dump the public keys.
      //
      {
	std::cout << alignment << Dumpable::Shift << "[Ks]" << std::endl;

	// dump the owner public key.
	std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		  << "[Owner]" << std::endl;

	if (this->Ks.owner.Dump(margin + 6) == StatusError)
	  escape("unable to dump the owner public key");

	// dump the author public key.
	std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		  << "[Author]" << std::endl;

	if (this->Ks.author.Dump(margin + 6) == StatusError)
	  escape("unable to dump the author public key");
      }

      //
      // dump the versions.
      //
      {
	std::cout << alignment << Dumpable::Shift << "[Versions]" << std::endl;

	// dump the data version.
	std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		  << "[Data] " << std::dec << this->versions.data << std::endl;

	// dump the meta version.
	std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		  << "[Meta] " << std::dec << this->versions.meta << std::endl;
      }

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the state.
    ///
    Status		State::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->genre,
			    this->stamps.creation,
			    this->stamps.modification,
			    this->size,
			    this->Ks.owner,
			    this->Ks.author,
			    this->versions.data,
			    this->versions.meta) == StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the state.
    ///
    Status		State::Extract(Archive&			archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->genre,
			  this->stamps.creation,
			  this->stamps.modification,
			  this->size,
			  this->Ks.owner,
			  this->Ks.author,
			  this->versions.data,
			  this->versions.meta) == StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}

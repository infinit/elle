//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Memento.cc
//
// created       julien quintard   [sat may  1 21:19:13 2010]
// updated       julien quintard   [thu may  6 01:11:16 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Memento.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the memento files extension.
  ///
  const elle::String		Memento::Extension = ".mto";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a memento.
  ///
  elle::Status		Memento::Create(
			  const elle::String&			name,
			  const etoile::hole::Address&		address,
			  const elle::Address&			network)
  {
    enter();

    // set the attributes.
    this->name = name;
    this->address = address;
    this->network = network;

    leave();
  }

  ///
  /// this method seals the memento with the authority.
  ///
  elle::Status		Memento::Seal(const Authority&		authority)
  {
    enter();

    // sign the attributesr with the authority.
    if (authority.k->Sign(this->name, this->address, this->network,
			  this->signature) == elle::StatusError)
      escape("unable to sign the attributes with the authority");

    leave();
  }

  ///
  /// this method verifies the validity of the memento.
  ///
  elle::Status		Memento::Validate(const Authority&	authority)
    const
  {
    enter();

    // verify the signature.
    if (authority.K.Verify(this->signature,
			   this->name, this->address, this->network) !=
	elle::StatusTrue)
      false();

    true();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Memento, _(), _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a memento.
  ///
  elle::Status		Memento::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Memento] " << std::endl;

    // dump the name.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[Name] " << this->name << std::endl;

    // dump the address.
    if (this->address.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the address");

    // dump the network.
    if (this->network.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the network");

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  elle::Status		Memento::Serialize(elle::Archive&	archive) const
  {
    enter();

    // serialize the attributes.
    if (archive.Serialize(this->name,
			  this->address,
			  this->network) == elle::StatusError)
      escape("unable to serialize the attributes");

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Memento::Extract(elle::Archive&		archive)
  {
    enter();

    // extract the attributes.
    if (archive.Extract(this->name,
			this->address,
			this->network) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads a memento.
  ///
  elle::Status		Memento::Load(const elle::String&	name)
  {
    elle::String	path =
      Lune::Universes +
      elle::System::Path::Separator +
      name +
      elle::System::Path::Separator +
      name +
      Memento::Extension;

    enter();

    // load as usual, but taking care to pass the right path.
    if (Object<>::Load(path) == elle::StatusError)
      escape("unable to load the memento");

    leave();
  }

  ///
  /// this method stores a memento.
  ///
  elle::Status		Memento::Store(const elle::String&	name) const
  {
    elle::String	path =
      Lune::Universes +
      elle::System::Path::Separator +
      name +
      elle::System::Path::Separator +
      name +
      Memento::Extension;

    enter();

    // store as usual, but taking care to pass the right path.
    if (Object<>::Store(path) == elle::StatusError)
      escape("unable to store the memento");

    leave();
  }

}

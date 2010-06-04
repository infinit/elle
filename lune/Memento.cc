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
// updated       julien quintard   [fri may 28 18:11:36 2010]
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

  ///
  /// this method loads a memento.
  ///
  elle::Status		Memento::Load(const elle::String&	name)
  {
    elle::String	path =
      Lune::Environment == Lune::ModeUser ?
      Lune::User::Universes + elle::System::Path::Separator +
      name + Memento::Extension :
      Lune::System::Universes + elle::System::Path::Separator +
      name + Memento::Extension;
    elle::Region	region;

    enter();

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(elle::String((char*)region.contents,
					       region.size),
				  *this) == elle::StatusError)
      escape("unable to decode the object");

    leave();
  }

  ///
  /// this method stores a memento.
  ///
  elle::Status		Memento::Store(const elle::String&	name) const
  {
    elle::String	path =
      Lune::Environment == Lune::ModeUser ?
      Lune::User::Universes + elle::System::Path::Separator +
      name + Memento::Extension :
      Lune::System::Universes + elle::System::Path::Separator +
      name + Memento::Extension;
    elle::Region	region;
    elle::String	string;

    enter();

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::StatusError)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap((elle::Byte*)string.c_str(),
		    string.length()) == elle::StatusError)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::File::Write(path, region) == elle::StatusError)
      escape("unable to write the file's content");

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Memento, _());

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
			  this->network,
			  this->signature) == elle::StatusError)
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
			this->network,
			this->signature) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

}

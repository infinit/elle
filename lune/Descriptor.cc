//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Descriptor.cc
//
// created       julien quintard   [sat may  1 21:19:13 2010]
// updated       julien quintard   [fri may 20 21:50:23 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Descriptor.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the descriptor files extension.
  ///
  const elle::String		Descriptor::Extension = ".dsc";

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Descriptor::Descriptor():
    model(hole::ModelUnknown)
  {
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a descriptor.
  ///
  elle::Status		Descriptor::Create(const elle::String&	name,
					   const hole::Model&	model,
					   const nucleus::Address& root,
					   const elle::Address&	boot)
  {
    enter();

    // set the attributes.
    this->name = name;
    this->model = model;
    this->root = root;
    this->boot = boot;

    leave();
  }

  ///
  /// this method seals the descriptor with the authority.
  ///
  elle::Status		Descriptor::Seal(const Authority&	authority)
  {
    enter();

    // sign the attributesr with the authority.
    if (authority.k->Sign(this->name,
			  (elle::Natural8&)this->model,
			  this->root,
			  this->boot,
			  this->signature) == elle::StatusError)
      escape("unable to sign the attributes with the authority");

    leave();
  }

  ///
  /// this method verifies the validity of the descriptor.
  ///
  elle::Status		Descriptor::Validate(const Authority&	authority)
    const
  {
    enter();

    // verify the signature.
    if (authority.K.Verify(this->signature,
			   this->name,
			   (elle::Natural8&)this->model,
			   this->root,
			   this->boot) != elle::StatusTrue)
      false();

    true();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Descriptor, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a descriptor.
  ///
  elle::Status		Descriptor::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Descriptor] " << std::endl;

    // dump the name.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[Name] " << this->name << std::endl;

    // dump the model.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[Model] " << (elle::Natural8)this->model << std::endl;

    // dump the root directory.
    std::cout << alignment << elle::Dumpable::Shift << "[Root] " << std::endl;

    // dump the address.
    if (this->root.Dump(margin + 4) == elle::StatusError)
      escape("unable to dump the address");

    // dump the boot.
    std::cout << alignment << elle::Dumpable::Shift << "[Boot]" << std::endl;

    // dump the network.
    if (this->boot.Dump(margin + 4) == elle::StatusError)
      escape("unable to dump the network");

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  elle::Status		Descriptor::Serialize(elle::Archive&	archive) const
  {
    enter();

    // serialize the attributes.
    if (archive.Serialize(this->name,
			  (elle::Natural8&)this->model,
			  this->root,
			  this->boot,
			  this->signature) == elle::StatusError)
      escape("unable to serialize the attributes");

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Descriptor::Extract(elle::Archive&	archive)
  {
    enter();

    // extract the attributes.
    if (archive.Extract(this->name,
			(elle::Natural8&)this->model,
			this->root,
			this->boot,
			this->signature) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads a descriptor.
  ///
  elle::Status		Descriptor::Load(const elle::String&	name)
  {
    elle::Path		path;
    elle::Region	region;

    enter();

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
      escape("unable to complete the path");

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
  /// this method stores a descriptor.
  ///
  elle::Status		Descriptor::Store(const elle::String&	name) const
  {
    elle::Path		path;
    elle::Region	region;
    elle::String	string;

    enter();

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
      escape("unable to complete the path");

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

  ///
  /// this method erases the descriptor.
  ///
  elle::Status		Descriptor::Erase(const elle::String&	name) const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    leave();
  }

  ///
  /// this method tests the descriptor.
  ///
  elle::Status		Descriptor::Exist(const elle::String&	name) const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      false();

    true();
  }

}

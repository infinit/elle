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
// updated       julien quintard   [thu jul 28 15:10:12 2011]
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
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a descriptor.
  ///
  elle::Status		Descriptor::Create(const elle::String&	name,
					   const hole::Model&	model,
					   const nucleus::Address& root)
  {
    enter();

    // set the attributes.
    this->name = name;
    this->model = model;
    this->root = root;

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
			  this->model,
			  this->root,
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
			   this->model,
			   this->root) == elle::StatusError)
      escape("unable to verify the signature");

    leave();
  }

  ///
  /// this method synchronises the in-memory descriptor so as to
  /// be stored.
  ///
  elle::Status		Descriptor::Push()
  {
    enter();

    //
    // update the settings with the parameters.
    //

    if (elle::Settings::Set(
	  "general", "name",
	  this->name) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "general", "model",
	  this->model) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "general", "root",
	  this->root) == elle::StatusError)
      escape("unable to update the parameter");

    if (elle::Settings::Set(
	  "general", "signature",
	  this->signature) == elle::StatusError)
      escape("unable to update the parameter");

    leave();
  }

  ///
  /// this method updates the in-memory parameters according to the
  /// associated settings.
  ///
  elle::Status		Descriptor::Pull()
  {
    enter();

    //
    // retrieve the parameters from the settings.
    //

    if (elle::Settings::Get(
	  "general", "name",
	  this->name) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "general", "model",
	  this->model) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "general", "root",
	  this->root) == elle::StatusError)
      escape("unable to retrieve the parameter");

    if (elle::Settings::Get(
	  "general", "signature",
	  this->signature) == elle::StatusError)
      escape("unable to retrieve the parameter");

    leave();
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
  /// note that this method may actually not dump the current values of
  /// the parameters.
  ///
  elle::Status		Descriptor::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Descriptor]" << std::endl;

    // dump the parent settings.
    if (elle::Settings::Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the settings");

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

    enter();

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // call the setting's method.
    if (elle::Settings::Load(path) == elle::StatusError)
      escape("unable to load the settings");

    leave();
  }

  ///
  /// this method stores a descriptor.
  ///
  elle::Status		Descriptor::Store(const elle::String&	name) const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // call the setting's method.
    if (elle::Settings::Store(path) == elle::StatusError)
      escape("unable to store the settings");

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

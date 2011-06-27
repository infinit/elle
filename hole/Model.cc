//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Model.cc
//
// created       julien quintard   [mon jun 27 09:15:33 2011]
// updated       julien quintard   [mon jun 27 10:33:56 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Model.hh>

namespace hole
{

//
// ---------- macro-functions -------------------------------------------------
//

///
/// XXX
///
#define ModelDefine(_identifier_)					\
  { Model::Type ## _identifier_, #_identifier_ }

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// XXX
  ///
  const Model				Model::Null;

  ///
  /// XXX
  ///
  const Model::Descriptor		Model::Descriptors[Model::Types] =
    {
      ModelDefine(Local),
      ModelDefine(Remote),
      ModelDefine(Kool)
    };

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Model::Model():
    type(Model::TypeUnknown)
  {
  }

  ///
  /// specific constructor.
  ///
  Model::Model(const Type					type):
    type(type)
  {
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  elle::Status		Model::Create(const elle::String&	name)
  {
    elle::Natural32	i;

    enter();

    // go through the descriptors.
    for (i = 0; i < Model::Types; i++)
      {
	// is this the model we are looking for?
	if (Model::Descriptors[i].name == name)
	  {
	    // set the model type.
	    this->type = Model::Descriptors[i].type;

	    leave();
	  }
      }

    escape("unable to locate the given model name");
  }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Model::operator==(const Model&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // check the attributes.
      if (this->type != element.type)
	false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Model, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an model object.
    ///
    elle::Status	Model::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      // display the name.
      std::cout << alignment << "[Model] " << this->type << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the model object.
    ///
    elle::Status	Model::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the internal digest.
      if (archive.Serialize((elle::Natural8&)this->type) == elle::StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the model object.
    ///
    elle::Status	Model::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the internal digest.
      if (archive.Extract((elle::Natural8&)this->type) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

}

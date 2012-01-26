//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [mon jun 27 09:15:33 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Model.hh>

#include <elle/idiom/Close.hh>
# include <algorithm>
# include <string>
#include <elle/idiom/Open.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this constants defines a null model.
  ///
  const Model                           Model::Null;

  ///
  /// this table maintains a mapping between model identifiers and
  /// human-readable representations.
  ///
  const Model::Descriptor               Model::Descriptors[Model::Types] =
    {
      { Model::TypeLocal, "local" },
      { Model::TypeRemote, "remote" },
      { Model::TypeSlug, "slug" },
      { Model::TypeCirkle, "cirkle" },
      { Model::TypeKool, "kool" },
    };

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method returns the model type associated with the given string.
  ///
  elle::Status          Model::Convert(const elle::String&      name,
                                       Type&                    type)
  {
    elle::String        string(name);
    elle::Natural32     i;

    enter();

    // transform the given name in lowercase.
    std::transform(string.begin(), string.end(),
                   string.begin(), std::ptr_fun(::tolower));

    // go through the descriptors.
    for (i = 0; i < Model::Types; i++)
      {
        // is this the model we are looking for?
        if (Model::Descriptors[i].name == string)
          {
            // set the model type.
            type = Model::Descriptors[i].type;

            leave();
          }
      }

    escape("unable to locate the given model name");
  }

  ///
  /// this method converts a type into its human-readable representation.
  ///
  elle::Status          Model::Convert(const Type               type,
                                       elle::String&            name)
  {
    elle::Natural32     i;

    enter();

    // go through the descriptors.
    for (i = 0; i < Model::Types; i++)
      {
        // is this the model we are looking for?
        if (Model::Descriptors[i].type == type)
          {
            // set the model name.
            name = Model::Descriptors[i].name;

            leave();
          }
      }

    escape("unable to locate the given model type");
  }

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
  Model::Model(const Type                                       type):
    type(type)
  {
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a model of the given type.
  ///
  elle::Status          Model::Create(const Type                type)
  {
    enter();

    // set the type.
    this->type = type;

    leave();
  }

  ///
  /// this method creates a model given its human-readable representation.
  ///
  elle::Status          Model::Create(const elle::String&       name)
  {
    enter();

    // convert the name into a type.
    if (Model::Convert(name, this->type) == elle::StatusError)
      escape("unable to convert the model name into a valid type");

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Model::operator==(const Model&  element) const
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
    elle::Status        Model::Dump(elle::Natural32             margin) const
    {
      elle::String      alignment(margin, ' ');

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
    elle::Status        Model::Serialize(elle::Archive& archive) const
    {
      enter();

      // serialize the internal digest.
      if (archive.Serialize(static_cast<elle::Natural8>(this->type)) ==
          elle::StatusError)
        escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the model object.
    ///
    elle::Status        Model::Extract(elle::Archive&           archive)
    {
      enter();

      // extract the internal digest.
      if (archive.Extract(reinterpret_cast<elle::Natural8&>(this->type)) ==
          elle::StatusError)
        escape("unable to extract the attributes");

      leave();
    }

}

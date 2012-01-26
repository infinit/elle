//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu aug 11 14:24:46 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Label.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this variable can easily be used for comparing with invalid labels.
  ///
  const Label                   Label::Null;

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// this method initializes the object.
  ///
  Label::Label():
    digest(NULL)
  {
  }

  ///
  /// this is the copy constructor.
  ///
  Label::Label(const Label&                             label):
    Object(label)
  {
    // copy the digest, if present.
    if (label.digest != NULL)
      {
        this->digest = new elle::Digest(*label.digest);
      }
    else
      {
        this->digest = NULL;
      }
  }

  ///
  /// this method reinitializes the object.
  ///
  Label::~Label()
  {
    // release the resources.
    if (this->digest != NULL)
      delete this->digest;
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this operator compares two objects.
  ///
  elle::Boolean         Label::operator==(const Label&  element) const
  {
    enter();

    // check the label as this may actually be the same object.
    if (this == &element)
      true();

    // if both are NULL or equal return true, false otherwise
    if ((this->digest == NULL) || (element.digest == NULL))
      {
        if (this->digest != element.digest)
          false();
      }
    else
      {
        if (*this->digest != *element.digest)
          false();
      }

    true();
  }

  ///
  /// this operator compares two objects.
  ///
  elle::Boolean         Label::operator<(const Label&   element) const
  {
    enter();

    // check the address as this may actually be the same object.
    if (this == &element)
      false();

    // test for a null digest.
    if ((this->digest == NULL) && (element.digest == NULL))
      false();
    else if (this->digest == NULL)
      true();
    else if (element.digest == NULL)
      false();

    // compare the digests.
    if (*this->digest < *element.digest)
      true();

    false();
  }

  ///
  /// this macro-function call generates the object.
  ///
  embed(Label, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this function dumps an label object.
  ///
  elle::Status          Label::Dump(elle::Natural32             margin) const
  {
    elle::String        alignment(margin, ' ');

    enter();

    // display the name.
    std::cout << alignment << "[Label]" << std::endl;

    // display the label depending on its value.
    if (*this == Label::Null)
      {
        std::cout << alignment << elle::Dumpable::Shift
                  << "[Digest] " << elle::none << std::endl;
      }
    else
      {
        // dump the digest.
        if (this->digest->Dump(margin + 2) == elle::StatusError)
          escape("unable to dump the digest");
      }

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the label object.
  ///
  elle::Status          Label::Serialize(elle::Archive& archive) const
  {
    enter();

    if (this->digest != NULL)
      {
        // serialize the internal digest.
        if (archive.Serialize(*this->digest) == elle::StatusError)
          escape("unable to serialize the digest");
      }
    else
      {
        // serialize 'none'.
        if (archive.Serialize(elle::none) == elle::StatusError)
          escape("unable to serialize 'none'");
      }

    leave();
  }

  ///
  /// this method extracts the label object.
  ///
  elle::Status          Label::Extract(elle::Archive&           archive)
  {
    elle::Archive::Type type;

    enter();

    // fetch the next element's type.
    if (archive.Fetch(type) == elle::StatusError)
      escape("unable to fetch the next element's type");

    if (type == elle::Archive::TypeNull)
      {
        // nothing to do, keep the digest to NULL.
        if (archive.Extract(elle::none) == elle::StatusError)
          escape("unable to extract null");
      }
    else
      {
        // allocate a digest.
        this->digest = new elle::Digest;

        // extract the internal digest.
        if (archive.Extract(*this->digest) == elle::StatusError)
          escape("unable to extract the digest");
      }

    leave();
  }

}

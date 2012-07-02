
#include <elle/standalone/Log.hh>

#include <hole/Label.hh>

#include <elle/idiom/Open.hh>

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
        this->digest = new elle::cryptography::Digest(*label.digest);
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
    // check the label as this may actually be the same object.
    if (this == &element)
      return true;

    // if both are NULL or equal return true, false otherwise
    if ((this->digest == NULL) || (element.digest == NULL))
      {
        if (this->digest != element.digest)
          return false;
      }
    else
      {
        if (*this->digest != *element.digest)
          return false;
      }

    return true;
  }

  ///
  /// this operator compares two objects.
  ///
  elle::Boolean         Label::operator<(const Label&   element) const
  {
    // check the address as this may actually be the same object.
    if (this == &element)
      return false;

    // test for a null digest.
    if ((this->digest == NULL) && (element.digest == NULL))
      return false;
    else if (this->digest == NULL)
      return true;
    else if (element.digest == NULL)
      return false;

    // compare the digests.
    if (*this->digest < *element.digest)
      return true;

    return false;
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

    // display the name.
    std::cout << alignment << "[Label]" << std::endl;

    // display the label depending on its value.
    if (*this == Label::Null)
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Digest] " << elle::none << std::endl;
      }
    else
      {
        // dump the digest.
        if (this->digest->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the digest");
      }

    return elle::Status::Ok;
  }

}

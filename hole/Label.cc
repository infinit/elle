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
    digest(nullptr)
  {
  }

  ///
  /// this is the copy constructor.
  ///
  Label::Label(const Label&                             label):
    Object(label)
  {
    // copy the digest, if present.
    if (label.digest != nullptr)
      {
        this->digest = new elle::cryptography::Digest(*label.digest);
      }
    else
      {
        this->digest = nullptr;
      }
  }

  ///
  /// this method reinitializes the object.
  ///
  Label::~Label()
  {
    // release the resources.
    if (this->digest != nullptr)
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

    // if both are nullptr or equal return true, false otherwise
    if ((this->digest == nullptr) || (element.digest == nullptr))
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
    if ((this->digest == nullptr) && (element.digest == nullptr))
      return false;
    else if (this->digest == nullptr)
      return true;
    else if (element.digest == nullptr)
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

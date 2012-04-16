//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu aug 11 14:27:39 2011]
//

#ifndef HOLE_LABEL_HXX
#define HOLE_LABEL_HXX

namespace hole
{

//
// ---------- templates -------------------------------------------------------
//

  ///
  /// create the label based on an object by serializing it before
  /// hashing it.
  ///
  template <typename T>
  elle::Status          Label::Create(const T&                  parameter)
  {
    // release the previous digest.
    if (this->digest != NULL)
      delete this->digest;

    // allocate the digest object.
    this->digest = new elle::cryptography::Digest;

    // compute the digest based on the parameters including the family.
    if (elle::cryptography::OneWay::Hash(parameter,
                           *this->digest) == elle::Status::Error)
      escape("unable to hash the given parameter(s)");

    return elle::Status::Ok;
  }

}

#endif

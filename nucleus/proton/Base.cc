
#include <elle/standalone/Log.hh>

#include <nucleus/proton/Base.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a base according to a version and digest.
    ///
    elle::Status        Base::Create(const Version&             version,
                                     elle::cryptography::Digest const&        digest)
    {
      // set the attributes.
      this->version = version;
      this->digest = digest;

      return elle::Status::Ok;
    }

    ///
    /// this method creates a base according to the referenced mutable block.
    ///
    elle::Status        Base::Create(const MutableBlock&        block)
    {
      // set the version.
      this->version = block.version;

      // compute the block's digest.
      if (elle::cryptography::OneWay::Hash(block, this->digest) == elle::Status::Error)
        escape("unable to hash the mutable block");

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the given mutable block matches the
    /// base.
    ///
    elle::Status        Base::Match(const MutableBlock&         block) const
    {
      elle::cryptography::Digest      digest;

      // check the versions.
      if (this->version != block.version)
        return elle::Status::False;

      // compute the block's digest.
      if (elle::cryptography::OneWay::Hash(block, digest) == elle::Status::Error)
        flee("unable to hash the mutable block");

      // compare the digests.
      if (this->digest != digest)
        return elle::Status::False;

      return elle::Status::True;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Base::operator==(const Base&            element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the attributes.
      if ((this->version != element.version) ||
          (this->digest != element.digest))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Base, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a base.
    ///
    elle::Status        Base::Dump(elle::Natural32              margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Base]" << std::endl;

      // dump the version.
      if (this->version.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the version");

      // dump the digest.
      if (this->digest.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the digest");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the base.
    ///
    //elle::Status        Base::Serialize(elle::Archive&          archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(this->version,
    //                        this->digest) == elle::Status::Error)
    //    escape("unable to serialize the attributes");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the base.
    /////
    //elle::Status        Base::Extract(elle::Archive&            archive)
    //{
    //  // extract the attributes.
    //  if (archive.Extract(this->version,
    //                      this->digest) == elle::Status::Error)
    //    escape("unable to extract the attributes");

    //  return elle::Status::Ok;
    //}

  }
}

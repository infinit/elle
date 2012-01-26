//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri jun 17 14:34:48 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Base.hh>
#include <nucleus/proton/MutableBlock.hh>

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
                                     const elle::Digest&        digest)
    {
      enter();

      // set the attributes.
      this->version = version;
      this->digest = digest;

      leave();
    }

    ///
    /// this method creates a base according to the referenced mutable block.
    ///
    elle::Status        Base::Create(const MutableBlock&        block)
    {
      enter();

      // set the version.
      this->version = block.version;

      // compute the block's digest.
      if (elle::OneWay::Hash(block, this->digest) == elle::StatusError)
        escape("unable to hash the mutable block");

      leave();
    }

    ///
    /// this method returns true if the given mutable block matches the
    /// base.
    ///
    elle::Status        Base::Match(const MutableBlock&         block) const
    {
      elle::Digest      digest;

      enter();

      // check the versions.
      if (this->version != block.version)
        false();

      // compute the block's digest.
      if (elle::OneWay::Hash(block, digest) == elle::StatusError)
        flee("unable to hash the mutable block");

      // compare the digests.
      if (this->digest != digest)
        false();

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Base::operator==(const Base&            element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
        true();

      // compare the attributes.
      if ((this->version != element.version) ||
          (this->digest != element.digest))
        false();

      true();
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

      enter();

      std::cout << alignment << "[Base]" << std::endl;

      // dump the version.
      if (this->version.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the version");

      // dump the digest.
      if (this->digest.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the digest");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the base.
    ///
    elle::Status        Base::Serialize(elle::Archive&          archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->version,
                            this->digest) == elle::StatusError)
        escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the base.
    ///
    elle::Status        Base::Extract(elle::Archive&            archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->version,
                          this->digest) == elle::StatusError)
        escape("unable to extract the attributes");

      leave();
    }

  }
}

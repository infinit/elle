#include <nucleus/proton/Base.hh>
#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/Exception.hh>

#include <elle/standalone/Log.hh>
#include <elle/cryptography/OneWay.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- construction ----------------------------------------------------
//

    Base::Base()
    {
    }

    Base::Base(MutableBlock const& block):
      _revision(block.revision())
    {
      // compute the block's digest.
      if (elle::cryptography::OneWay::Hash(block, this->_digest)
          == elle::Status::Error)
        throw Exception("unable to hash the mutable block");
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a base according to a revision and digest.
    ///
    elle::Status        Base::Create(const Revision&             revision,
                                     elle::cryptography::Digest const&        digest)
    {
      // set the attributes.
      this->_revision = revision;
      this->_digest = digest;

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the given mutable block matches the
    /// base.
    ///
    elle::Status        Base::Match(const MutableBlock&         block) const
    {
      elle::cryptography::Digest      digest;

      // check the revisions.
      if (this->_revision != block.revision())
        return elle::Status::False;

      // compute the block's digest.
      if (elle::cryptography::OneWay::Hash(
            block,
            digest) == elle::Status::Error
          )
        flee("unable to hash the mutable block");

      // compare the digests.
      if (this->_digest != digest)
        return elle::Status::False;

      return elle::Status::True;
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Base::operator==(Base const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the attributes.
      if ((this->_revision != other._revision) ||
          (this->_digest != other._digest))
        return false;

      return true;
    }

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

      // dump the revision.
      if (this->_revision.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the revision");

      // dump the digest.
      if (this->_digest.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the digest");

      return elle::Status::Ok;
    }

  }
}

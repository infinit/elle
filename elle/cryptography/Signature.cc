//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon feb  2 22:22:12 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Signature.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// this defines a null signature.
    ///
    const Signature             Signature::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Signature::Signature()
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two signatures match.
    ///
    Boolean             Signature::operator==(const Signature&  element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
        true();

      // compare the regions.
      if (this->region != element.region)
        false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Signature, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the signature.
    ///
    Status              Signature::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');

      enter();

      // display depending on the value.
      if (*this == Signature::Null)
        {
          std::cout << alignment << "[Signature] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Signature] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == StatusError)
            escape("unable to dump the secret key");
        }

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a signature object.
    ///
    Status              Signature::Serialize(Archive&           archive) const
    {
      enter();

      // serialize the region.
      if (archive.Serialize(this->region) == StatusError)
        escape("unable to serialize the region");

      leave();
    }

    ///
    /// this method extracts a signature object.
    ///
    Status              Signature::Extract(Archive&             archive)
    {
      enter();

      // extract the content.
      if (archive.Extract(this->region) == StatusError)
        escape("unable to extract the region");

      leave();
    }

  }
}

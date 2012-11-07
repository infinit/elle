#include <cryptography/Signature.hh>

namespace infinit
{
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
    elle::Boolean             Signature::operator==(const Signature&  element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->region == element.region);
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
    elle::Status              Signature::Dump(const elle::Natural32         margin) const
    {
      elle::String            alignment(margin, ' ');

      // display depending on the value.
      if (*this == Signature::Null)
        {
          std::cout << alignment << "[Signature] " << elle::none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Signature] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the secret key");
        }

      return elle::Status::Ok;
    }

  }
}

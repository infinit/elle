#include <elle/cryptography/Signature.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

namespace elle
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
    Boolean             Signature::operator==(const Signature&  element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the regions.
      if (this->region != element.region)
        return false;

      return true;
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

      // display depending on the value.
      if (*this == Signature::Null)
        {
          std::cout << alignment << "[Signature] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Signature] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == Status::Error)
            escape("unable to dump the secret key");
        }

      return Status::Ok;
    }

  }
}

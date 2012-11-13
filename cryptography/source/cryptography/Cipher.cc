#include <cryptography/Cipher.hh>

namespace infinit
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null cipher.
    ///
    const Cipher                Cipher::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Cipher::Cipher()
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two ciphers match.
    ///
    elle::Boolean             Cipher::operator==(const Cipher&        element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->region == element.region);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Cipher, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the cipher.
    ///
    elle::Status              Cipher::Dump(const elle::Natural32            margin) const
    {
      elle::String            alignment(margin, ' ');

      // display the cipher depending on its value.
      if (*this == Cipher::Null)
        {
          std::cout << alignment << "[Cipher] " << elle::none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Cipher] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the region");
        }

      return elle::Status::Ok;
    }

  }
}

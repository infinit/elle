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

#include <elle/cryptography/Cipher.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace package;

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
    Boolean             Cipher::operator==(const Cipher&        element) const
    {
      ;

      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::StatusTrue;

      // compare the regions.
      if (this->region != element.region)
        return elle::StatusFalse;

      return elle::StatusTrue;
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
    Status              Cipher::Dump(const Natural32            margin) const
    {
      String            alignment(margin, ' ');

      ;

      // display the cipher depending on its value.
      if (*this == Cipher::Null)
        {
          std::cout << alignment << "[Cipher] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Cipher] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == StatusError)
              escape("unable to dump the region");
        }

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a cipher object.
    ///
    Status              Cipher::Serialize(Archive&              archive) const
    {
      ;

      // serialize the region.
      if (archive.Serialize(this->region) == StatusError)
        escape("unable to serialize the region");

      return elle::StatusOk;
    }

    ///
    /// this method extracts a cipher object.
    ///
    Status              Cipher::Extract(Archive&                archive)
    {
      ;

      // extract the content.
      if (archive.Extract(this->region) == StatusError)
        escape("unable to extract the region");

      return elle::StatusOk;
    }

  }
}

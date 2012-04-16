
#include <elle/cryptography/Cipher.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

namespace elle
{

  using namespace standalone;

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
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return this->region != element.region;
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

      // display the cipher depending on its value.
      if (*this == Cipher::Null)
        {
          std::cout << alignment << "[Cipher] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Cipher] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == Status::Error)
              escape("unable to dump the region");
        }

      return Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a cipher object.
    ///
    //Status              Cipher::Serialize(Archive&              archive) const
    //{
    //  // serialize the region.
    //  if (archive.Serialize(this->region) == Status::Error)
    //    escape("unable to serialize the region");

    //  return Status::Ok;
    //}

    /////
    ///// this method extracts a cipher object.
    /////
    //Status              Cipher::Extract(Archive&                archive)
    //{
    //  // extract the content.
    //  if (archive.Extract(this->region) == Status::Error)
    //    escape("unable to extract the region");

    //  return Status::Ok;
    //}

  }
}

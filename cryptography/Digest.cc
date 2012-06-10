#include <elle/cryptography/Digest.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null digest.
    ///
    const Digest                Digest::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Digest::Digest()
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two digests match.
    ///
    Boolean             Digest::operator==(const Digest&        element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->region == element.region);
    }

    ///
    /// this method compares the digests.
    ///
    Boolean             Digest::operator<(const Digest&         element) const
    {
      return (this->region < element.region);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Digest, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the digest.
    ///
    Status              Digest::Dump(const Natural32            margin) const
    {
      String            alignment(margin, ' ');

      // display depending on the value.
      if (*this == Digest::Null)
        {
          std::cout << alignment << "[Digest] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Digest]" << std::endl;

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
    /// this method serializes a digest object.
    ///
    //Status              Digest::Serialize(Archive&              archive) const
    //{
    //  // serialize the region.
    //  if (archive.Serialize(this->region) == Status::Error)
    //    escape("unable to serialize the region");

    //  return Status::Ok;
    //}

    /////
    ///// this method extracts a digest object.
    /////
    //Status              Digest::Extract(Archive&                archive)
    //{
    //  // extract the content.
    //  if (archive.Extract(this->region) == Status::Error)
    //    escape("unable to extract the region");

    //  return Status::Ok;
    //}

  }
}

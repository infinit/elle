
#include <elle/cryptography/Code.hh>

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
    /// this defines a null code.
    ///
    const Code                  Code::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Code::Code()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a code based on the given region.
    ///
    Status              Code::Create(const Region&      region)
    {
      // set the region.
      this->region = region;

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two codes match.
    ///
    Boolean             Code::operator==(const Code&    element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->region == element.region);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Code, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the code.
    ///
    Status              Code::Dump(const Natural32              margin) const
    {
      String            alignment(margin, ' ');

      // display depending on the value.
      if (*this == Code::Null)
        {
          std::cout << alignment << "[Code] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Code] " << std::endl;

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
    /// this method serializes a code object.
    ///
    //Status              Code::Serialize(Archive&                archive) const
    //{
    //  // serialize the region.
    //  if (archive.Serialize(this->region) == Status::Error)
    //    escape("unable to serialize the region");

    //  return Status::Ok;
    //}

    /////
    ///// this method extracts a code object.
    /////
    //Status              Code::Extract(Archive&          archive)
    //{
    //  // extract the content.
    //  if (archive.Extract(this->region) == Status::Error)
    //    escape("unable to extract the region");

    //  return Status::Ok;
    //}

  }
}

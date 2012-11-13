#include <cryptography/Code.hh>

namespace infinit
{
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
    elle::Status              Code::Create(const elle::standalone::Region& region)
    {
      // set the region.
      this->region = region;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two codes match.
    ///
    elle::Boolean             Code::operator==(const Code&    element) const
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
    elle::Status              Code::Dump(const elle::Natural32              margin) const
    {
      elle::String            alignment(margin, ' ');

      // display depending on the value.
      if (*this == Code::Null)
        {
          std::cout << alignment << "[Code] " << elle::none << std::endl;
        }
      else
        {
          std::cout << alignment << "[Code] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the region");
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Code::print(std::ostream& stream) const
    {
      stream << "code("
             << this->region.size
             << ")";
    }

  }
}

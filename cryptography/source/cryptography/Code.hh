#ifndef INFINIT_CRYPTOGRAPHY_CODE_HH
# define INFINIT_CRYPTOGRAPHY_CODE_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

# include <elle/standalone/Region.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Code:
      public elle::Printable,
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Code         Null;

      //
      // constructors & destructors
      //
      Code();

      //
      // methods
      //
      elle::Status            Create(const elle::standalone::Region&);

      //
      // interfaces
      //

      // object
      declare(Code);
      elle::Boolean           operator==(const Code&) const;
      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
      elle::standalone::Region            region;
    };

  }
}

#include <cryptography/Code.hxx>

#endif

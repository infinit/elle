#ifndef ELLE_CRYPTOGRAPHY_CODE_HH
# define ELLE_CRYPTOGRAPHY_CODE_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

# include <elle/standalone/Region.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Code:
      public Printable,
      public radix::Object
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
      Status            Create(const standalone::Region&);

      //
      // interfaces
      //

      // object
      declare(Code);
      Boolean           operator==(const Code&) const;
      // dumpable
      Status            Dump(const Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
      standalone::Region            region;
    };

  }
}

#include <elle/cryptography/Code.hxx>

#endif

#ifndef ELLE_CRYPTOGRAPHY_CODE_HH
# define ELLE_CRYPTOGRAPHY_CODE_HH

//
// ---------- includes --------------------------------------------------------
//

# include <elle/types.hh>

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

      //
      // attributes
      //
      standalone::Region            region;
    };

  }
}

#endif
#include <elle/cryptography/Code.hxx>

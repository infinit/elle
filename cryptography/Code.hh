//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/Code.hh
//
// created       julien quintard   [tue oct 30 01:16:28 2007]
// updated       julien quintard   [fri may 28 12:16:55 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_CODE_HH
#define ELLE_CRYPTOGRAPHY_CODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/archive/Archive.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace archive;

  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Code:
      public Object
    {
    public:
      //
      // constants
      //
      static const Code		Null;

      //
      // interfaces
      //

      // object
      declare(Code);
      Boolean		operator==(const Code&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Region		region;
    };

  }
}

#endif

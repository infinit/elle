//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Directory.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [thu may  5 16:26:58 2011]
//

#ifndef ETOILE_CONTEXT_DIRECTORY_HH
#define ETOILE_CONTEXT_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/context/Object.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this context represents a directory object as it embeds
    /// a catalog contents along with inherited object-related stuff.
    ///
    class Directory:
      public Object
    {
    public:
      //
      // types
      //
      typedef nucleus::Catalog		Content;

      //
      // constructors & destructors
      //
      Directory();
      ~Directory();

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::Contents<Content>*	contents;
    };

  }
}

#endif

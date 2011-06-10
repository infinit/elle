//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Directory.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [fri jun  3 11:38:13 2011]
//

#ifndef ETOILE_GEAR_DIRECTORY_HH
#define ETOILE_GEAR_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Object.hh>

namespace etoile
{
  namespace gear
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

      // XXX object

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // XXX archivable

      //
      // attributes
      //
      nucleus::Contents<Content>*	contents;
    };

  }
}

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Unit.hh
//
// created       julien quintard   [tue jan 26 14:00:35 2010]
// updated       julien quintard   [thu may  5 15:58:23 2011]
//

#ifndef ETOILE_DEPOT_UNIT_HH
#define ETOILE_DEPOT_UNIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class holds a block in the reserve i.e on a more stable but
    /// slower storage medium that main memory such as a hard disk.
    ///
    class Unit:
      public elle::Entity
    {
    public:
      //
      // methods
      //
      elle::Status	Load(const elle::String);

      elle::Status	Set(nucleus::Block*);
      elle::Status	Get(nucleus::Block*);
      elle::Status	Destroy();

      //
      // interfaces
      //
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::String	path;
      elle::Natural32	size;
    };

  }
}

#endif

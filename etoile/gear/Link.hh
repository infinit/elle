//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Link.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [fri jun 17 16:24:26 2011]
//

#ifndef ETOILE_GEAR_LINK_HH
#define ETOILE_GEAR_LINK_HH

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
    /// this context represents a link object as it embeds
    /// a reference along with inherited object-related stuff.
    ///
    class Link:
      public Object
    {
    public:
      //
      // constructors & destructors
      //
      Link();
      ~Link();

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
      nucleus::Contents<nucleus::Reference>*	contents;
    };

  }
}

#endif

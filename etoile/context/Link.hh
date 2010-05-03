//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Link.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [mon may  3 12:52:26 2010]
//

#ifndef ETOILE_CONTEXT_LINK_HH
#define ETOILE_CONTEXT_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>

#include <etoile/kernel/Contents.hh>
#include <etoile/kernel/Catalog.hh>

namespace etoile
{
  namespace context
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
      // types
      //
      typedef kernel::Reference		Content;

      //
      // constructors & destructors
      //
      Link();
      ~Link();

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      kernel::Contents<Content>*	contents;
    };

  }
}

#endif

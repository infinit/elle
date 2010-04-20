//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Link.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [tue apr 20 10:31:18 2010]
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
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      kernel::Contents<Content>*	contents;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Format.hh>

#endif

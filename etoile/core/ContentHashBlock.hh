//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/ContentHashBlock.hh
//
// created       julien quintard   [tue feb 17 12:33:12 2009]
// updated       julien quintard   [sat aug  1 15:59:11 2009]
//

#ifndef ETOILE_COMPONENTS_CONTENTHASHBLOCK_HH
#define ETOILE_COMPONENTS_CONTENTHASHBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Block.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class ContentHashBlock:
      public Block
    {
    public:
      //
      // methods
      //
      Status		Seal();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

#endif

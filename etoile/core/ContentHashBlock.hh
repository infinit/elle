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
// updated       julien quintard   [thu jan 28 13:22:16 2010]
//

#ifndef ETOILE_CORE_CONTENTHASHBLOCK_HH
#define ETOILE_CORE_CONTENTHASHBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>

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
      public hole::Block
    {
    public:
      //
      // constructors & destructors
      //
      ContentHashBlock();

      //
      // methods
      //
      Status		Bind();
      Status		Validate(const hole::Address&) const;

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

#endif

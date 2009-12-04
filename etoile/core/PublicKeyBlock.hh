//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/PublicKeyBlock.hh
//
// created       julien quintard   [tue feb 17 19:45:45 2009]
// updated       julien quintard   [tue dec  1 02:58:06 2009]
//

#ifndef ETOILE_CORE_PUBLICKEYBLOCK_HH
#define ETOILE_CORE_PUBLICKEYBLOCK_HH

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
    class PublicKeyBlock:
      public hole::Block
    {
    public:
      //
      // constructors & destructors
      //
      PublicKeyBlock(const hole::Block::Kind);

      //
      // methods
      //
      Status		Create(const KeyPair&);

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

      //
      // attributes
      //
      PublicKey		K;
    };

  }
}

#endif

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
// updated       julien quintard   [tue aug  4 13:56:56 2009]
//

#ifndef ETOILE_COMPONENTS_PUBLICKEYBLOCK_HH
#define ETOILE_COMPONENTS_PUBLICKEYBLOCK_HH

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
    class PublicKeyBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      PublicKeyBlock();
      ~PublicKeyBlock();

      //
      // methods
      //
      Status		Create();

      Status		Seal();

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

      ///
      /// this private key is kept after the PKB creation
      /// but is not serialized, hence is optional.
      ///
      PrivateKey*	k;
    };

  }
}

#endif

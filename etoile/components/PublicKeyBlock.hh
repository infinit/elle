//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/infinit/components/PublicKeyBlock.hh
//
// created       julien quintard   [tue feb 17 19:45:45 2009]
// updated       julien quintard   [wed mar 11 13:33:17 2009]
//

#ifndef ETOILE_COMPONENTS_PUBLICKEYBLOCK_HH
#define ETOILE_COMPONENTS_PUBLICKEYBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Block.hh>

namespace etoile
{
  namespace components
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
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      PublicKeyBlock()
      {
	PublicKeyBlock::New(*this);
      }

      ~PublicKeyBlock()
      {
	PublicKeyBlock::Delete(*this);
      }

      //
      // methods
      //
      Status		Create();

      Status		Seal();

      //
      // interfaces
      //

      // object
      static Status	New(PublicKeyBlock&);
      static Status	Delete(PublicKeyBlock&);

      PublicKeyBlock&	operator=(const PublicKeyBlock&);
      Boolean		operator==(const PublicKeyBlock&);
      Boolean		operator!=(const PublicKeyBlock&);

      // dumpable
      Status		Dump(const Natural32 = 0);

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

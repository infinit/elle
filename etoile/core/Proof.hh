//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Proof.hh
//
// created       julien quintard   [sat jul 25 00:45:05 2009]
// updated       julien quintard   [sat mar 20 04:10:59 2010]
//

#ifndef ETOILE_CORE_PROOF_HH
#define ETOILE_CORE_PROOF_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Voucher.hh>

namespace etoile
{
  namespace core
  {

    ///
    /// this class defines a proof which is composed of an index
    /// referencing a delegate in the Access block and a voucher, signed
    /// by this delegate.
    ///
    /// note that no voucher is embedded should the author be a delegate.
    ///
    class Proof:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      Proof();
      ~Proof();

      //
      // methods
      //
      Status		Specify(const Natural32&);
      Status		Specify(const Natural32&,
				const Voucher&);

      //
      // interfaces
      //

      // entity
      declare(Entity, Proof);
      Boolean		operator==(const Proof&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Natural32		index;
      Voucher*		voucher;
    };

  }
}

#endif

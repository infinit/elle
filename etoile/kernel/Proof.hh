//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Proof.hh
//
// created       julien quintard   [sat jul 25 00:45:05 2009]
// updated       julien quintard   [fri apr  2 13:32:22 2010]
//

#ifndef ETOILE_KERNEL_PROOF_HH
#define ETOILE_KERNEL_PROOF_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Voucher.hh>

namespace etoile
{
  namespace kernel
  {

    ///
    /// this class defines a proof which is composed of an index
    /// referencing a delegate in the Access block and a voucher, signed
    /// by this identified delegate.
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
      Status		Specify(const Index&);
      Status		Specify(const Index&,
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
      Index		index;
      Voucher*		voucher;
    };

  }
}

#endif

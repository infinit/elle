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
// updated       julien quintard   [fri may 28 17:47:21 2010]
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
      public elle::Object
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
      elle::Status	Specify(const Index&);
      elle::Status	Specify(const Index&,
				const Voucher&);

      //
      // interfaces
      //

      // object
      declare(Proof);
      elle::Boolean	operator==(const Proof&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Index		index;
      Voucher*		voucher;
    };

  }
}

#endif

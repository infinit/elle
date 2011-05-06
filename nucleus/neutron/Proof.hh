//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Proof.hh
//
// created       julien quintard   [sat jul 25 00:45:05 2009]
// updated       julien quintard   [thu may  5 09:31:02 2011]
//

#ifndef NUCLEUS_NEUTRON_PROOF_HH
#define NUCLEUS_NEUTRON_PROOF_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Voucher.hh>

namespace nucleus
{
  namespace neutron
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

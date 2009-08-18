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
// updated       julien quintard   [tue aug  4 13:56:44 2009]
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
      Status		Specify(Natural32);
      Status		Specify(Natural32,
				Voucher&);

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
      Natural32		delegate;
      Voucher*		voucher;
    };

  }
}

#endif

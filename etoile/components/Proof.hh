//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Proof.hh
//
// created       julien quintard   [sat jul 25 00:45:05 2009]
// updated       julien quintard   [mon jul 27 08:32:30 2009]
//

#ifndef ETOILE_COMPONENTS_PROOF_HH
#define ETOILE_COMPONENTS_PROOF_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Voucher.hh>

namespace etoile
{
  namespace components
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
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      Proof()
      {
	Proof::New(*this);
      }

      ~Proof()
      {
	Proof::Delete(*this);
      }

      //
      // methods
      //
      Status		Specify(Natural32);
      Status		Specify(Natural32,
				Voucher&);

      //
      // interfaces
      //

      // object
      static Status	New(Proof&);
      static Status	Delete(Proof&);

      Proof&		operator=(const Proof&);
      Boolean		operator==(const Proof&);
      Boolean		operator!=(const Proof&);

      // dumpable
      Status		Dump(const Natural32 = 0);

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

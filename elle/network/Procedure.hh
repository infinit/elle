//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Procedure.hh
//
// created       julien quintard   [fri jul 15 11:55:49 2011]
// updated       julien quintard   [mon jul 18 23:23:05 2011]
//

#ifndef ELLE_NETWORK_PROCEDURE_HH
#define ELLE_NETWORK_PROCEDURE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Parameters.hh>
#include <elle/radix/Trait.hh>
#include <elle/radix/Set.hh>

#include <elle/package/Archive.hh>

#include <elle/network/Message.hh>

#include <elle/concurrency/Callback.hh>

#include <elle/Manifest.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;
  using namespace concurrency;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <const Tag I,
	      const Tag O = TagNone>
    class Procedure:
      public Object
    {
    public:
      //
      // types
      //
      typedef
        typename
          Set::Union<
            typename
              Trait::Constant<
	        typename Message<I>::P
	      >::Type,
	    typename
	      Trait::Bare<
	        typename Message<O>::P
	      >::Type
            >::Type				R;

      //
      // constructors & destructors
      //
      Procedure(const Callback< Status,
				R >&,
		const Callback< Status,
				Parameters<> >* = NULL,
		const Callback< Status,
				Parameters<> >* = NULL);
      Procedure(const Procedure<I, O>&);

      //
      // methods
      //
      Status		Skeleton(Archive&) const;

      //
      // interfaces
      //

      // object
      declare(_(Procedure<I, O>));

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Callback<
	Status,
	R
	>			routine;

      Callback<
	Status,
	Parameters<>
	>*			prolog;
      Callback<
	Status,
	Parameters<>
	>*			epilog;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Procedure.hxx>

#endif

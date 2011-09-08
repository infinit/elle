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
// updated       julien quintard   [wed sep  7 14:25:04 2011]
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
    /// the Procedure class enables the programmer to associate a callback
    /// with a message whose tag is I.
    ///
    /// besides, the Procedure class goes further by enabling one to define
    /// an output message to send back to the caller. should this functionality
    /// be activated, by providing a tag O, the procedure's callback should
    /// accept both I's parameters as inputs and O's parameters as outputs.
    ///
    /// for instance, assuming a message TagChallenge takes a String and
    /// TagResponse take a Digest, the following Procedure could be defined:
    ///
    ///   Procedure<TagChallenge, TagResponse>(Callback<>::Infer(&Challenge));
    ///
    /// with the the Challenge() method:
    ///
    ///   Status	Challenge(const String&			string,
    ///				  Digest&			digest)
    ///   {
    ///     [...]
    ///   }
    ///
    /// this way, whenever a message TagChallenge is received, the Challenge()
    /// method is triggered. in addition, when the method returns, the
    /// Procedure takes the outputs, i.e _digest_, packs them and sends a
    /// TagResponse message back to the caller.
    ///
    /// finally, the programmer can also define an error tag, E, so that,
    /// should the callback return an error, an message is send back to
    /// the caller embedding a report i.e a set of error messages.
    ///
    template <const Tag I,
	      const Tag O = TagNone,
	      const Tag E = TagError>
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
				R >,
		const Callback< Status,
		                Parameters<> > =
		  Callback< Status,
			    Parameters<> >::Null,
		const Callback< Status,
				Parameters<> > =
		  Callback< Status,
			    Parameters<> >::Null);

      //
      // methods
      //
      Status		Skeleton(Archive&) const;

      //
      // interfaces
      //

      // object
      declare(_(Procedure<I, O, E>));

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Callback<
	Status,
	R
	>		routine;

      Callback<
	Status,
	Parameters<>
	>		prolog;
      Callback<
	Status,
	Parameters<>
	>		epilog;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Procedure.hxx>

#endif

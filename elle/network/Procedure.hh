#ifndef ELLE_NETWORK_PROCEDURE_HH
# define ELLE_NETWORK_PROCEDURE_HH

# include <elle/radix/Parameters.hh>
# include <elle/radix/Trait.hh>
# include <elle/radix/Set.hh>

# include <elle/network/fwd.hh>
# include <elle/network/Locus.hh>
# include <elle/network/Message.hh>

# include <elle/concurrency/Callback.hh>

# include <elle/Manifest.hh>

# include <protocol/Channel.hh>

namespace elle
{
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
    ///   Status        Challenge(const String&                 string,
    ///                           Digest&                       digest)
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
      public radix::Object
    {
    public:
      //
      // types
      //
      typedef
        typename
      radix::Set::Union<
            typename
      radix::Trait::Constant<
                typename Message<I>::P
              >::Type,
            typename Message<O>::P
            >::Type                             R;

      typedef concurrency::Callback<Status, radix::Parameters<>> BaseCallback;

      //
      // constructors & destructors
      //
      Procedure(const concurrency::Callback< Status,
                                R >,
                const BaseCallback = BaseCallback::Null,
                const BaseCallback = BaseCallback::Null);

      //
      // methods
      //
      Status            Skeleton(TCPSocket*, Locus&, infinit::protocol::Stream&, Parcel&) const;

      //
      // interfaces
      //

      // object
      declare(_(Procedure<I, O, E>));

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      concurrency::Callback<
        Status,
        R
        >               routine;

      concurrency::Callback<
        Status,
        radix::Parameters<>
        >               prolog;
      concurrency::Callback<
        Status,
        radix::Parameters<>
        >               epilog;
    };

  }
}

#include <elle/network/Procedure.hxx>

#endif

#ifndef ELLE_CONCURRENCY_EVENT_HH
# define ELLE_CONCURRENCY_EVENT_HH

# include <elle/types.hh>

# include <elle/serialize/fwd.hh>
# include <elle/network/fwd.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Close.hh>
#  include <reactor/semaphore.hh>
# include <elle/idiom/Open.hh>

namespace elle
{
  using namespace radix;

  namespace concurrency
  {

    ///
    /// this class is used to uniquely identify events, network packets and
    /// so on.
    ///
    class Event
      : public Object
    {
    public:
      //
      // constants
      //
      static const Event        Null;

      //
      // constructors & destructors
      //
      Event();

      //
      // methods
      //
      Status            Generate();
      void              Cleanup();

      //
      // interfaces
      //

      // object
      declare(Event);
      Boolean           operator==(const Event&) const;
      Boolean           operator<(const Event&) const;

      void            XXX_OLD_Extract();

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural64 Identifier() const;

    public:
      typedef reactor::Semaphore SignalType;
      bool waited;
      void Parcel(elle::network::Parcel*);
      elle::network::Parcel* Parcel();
      SignalType& Signal();
    private:
      elle::network::Parcel* parcel;
      Natural64                                 _identifier;
      SignalType*  _signal;

      ELLE_SERIALIZE_FRIEND_FOR(Event);
    };

  }
}

#include <elle/concurrency/Event.hxx>

#endif

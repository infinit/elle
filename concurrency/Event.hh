#ifndef ELLE_CONCURRENCY_EVENT_HH
# define ELLE_CONCURRENCY_EVENT_HH

# include <elle/core/Natural.hh>
# include <elle/core/Boolean.hh>

# include <elle/radix/Status.hh>
# include <elle/radix/Object.hh>

# include <elle/package/Archive.hh>

# include <elle/idiom/Close.hh>
#  include <openssl/err.h>
#  include <reactor/semaphore.hh>
# include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  // XXX
  namespace network
  {
    class Parcel;
  }

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

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

      // archivable
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

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
    };

  }
}

#endif

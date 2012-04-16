#ifndef ELLE_CONCURRENCY_EVENT_HH
# define ELLE_CONCURRENCY_EVENT_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Close.hh>
#  include <openssl/err.h>
# include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is used to uniquely identify events, network packets and
    /// so on.
    ///
    class Event:
      public Object
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

      //
      // interfaces
      //

      // object
      declare(Event);
      Boolean           operator==(const Event&) const;
      Boolean           operator<(const Event&) const;

      //// archivable
      //Status            Serialize(Archive&) const;
      //Status            Extract(Archive&);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural64         identifier;
    };

  }
}

#endif

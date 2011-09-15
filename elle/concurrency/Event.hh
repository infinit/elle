//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar  3 13:37:54 2010]
//

#ifndef ELLE_CONCURRENCY_EVENT_HH
#define ELLE_CONCURRENCY_EVENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Boolean.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/idiom/Close.hh>
# include <openssl/err.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

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
      static const Event	Null;

      //
      // constructors & destructors
      //
      Event();

      //
      // methods
      //
      Status		Generate();

      //
      // interfaces
      //

      // object
      declare(Event);
      Boolean		operator==(const Event&) const;
      Boolean		operator<(const Event&) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural64		identifier;
    };

  }
}

#endif

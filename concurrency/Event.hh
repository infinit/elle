//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Event.hh
//
// created       julien quintard   [wed mar  3 13:37:54 2010]
// updated       julien quintard   [wed mar 31 01:08:55 2010]
//

#ifndef ELLE_CONCURRENCY_EVENT_HH
#define ELLE_CONCURRENCY_EVENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/archive/Archive.hh>
#include <elle/io/IO.hh>

#include <elle/misc/Status.hh>

#include <elle/idiom/Close.hh>
# include <openssl/rand.h>
# include <openssl/err.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;
  using namespace io;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// events are used to uniquely identify events, network packets and
    /// so on.
    ///
    class Event:
      public Entity,
      public Dumpable, public Archivable
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

      // entity
      declare(Entity, Event);
      Boolean		operator==(const Event&) const;

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

//
// ---------- operators -------------------------------------------------------
//

    Boolean		operator<(const Event&,
				  const Event&);

  }
}

#endif

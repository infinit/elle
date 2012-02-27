//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon feb 22 23:48:40 2010]
//

#ifndef ELLE_NETWORK_HEADER_HH
#define ELLE_NETWORK_HEADER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/concurrency/Event.hh>

#include <elle/network/Tag.hh>

#include <elle/idiom/Open.hh>

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
    /// this class represents the header which is attached to every
    /// network message.
    ///
    class Header:
      public Object
    {
    public:
      //
      // constants
      //
      static const String               Name;

      //
      // constructors & destructors
      //
      Header();

      //
      // methods
      //
      Status            Create(const Event&,
                               const Tag,
                               const Natural32);

      //
      // interfaces
      //

      // object
      declare(Header);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

      //
      // attributes
      //
      Event             event;
      Tag               tag;
      Natural32         size;
    };

  }
}

#endif

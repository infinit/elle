//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri oct 16 04:10:36 2009]
//

#ifndef ELLE_NETWORK_HOST_HH
#define ELLE_NETWORK_HOST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <elle/types.hh>
#include <elle/types.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/idiom/Close.hh>
# include <QHostAddress>
# include <QString>
#include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;
  using namespace package;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a network host.
    ///
    class Host:
      public Object
    {
    public:
      //
      // constants
      //
      static const Host         Null;

      //
      // enumerations
      //
      enum Type
        {
          TypeNull,
          TypeAny,
          TypeIP,
          TypeBroadcast
        };

      //
      // types
      //
      typedef std::vector<Host>                         Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // static methods
      //
      static Status     Hosts(Container&);

      //
      // constructors & destructors
      //
      Host();

      //
      // methods
      //
      Status            Create(const Type);
      Status            Create(const String&);

      Status            Convert(String&) const;

      //
      // interfaces
      //

      // object
      declare(Host);
      Boolean           operator==(const Host&) const;
      Boolean           operator<(const Host&) const;
      Boolean           operator>(const Host&) const;

      // archivable
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type              type;
      ::QHostAddress    location;
    };

  }
}

#endif




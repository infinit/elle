#ifndef ELLE_NETWORK_HOST_HH
# define ELLE_NETWORK_HOST_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/idiom/Close.hh>
#  include <QHostAddress>
#  include <QString>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

    /// This class represents a network host.
    class Host:
      public elle::radix::Object
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

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Host();
      Host(Type type);

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

#include <elle/network/Host.hxx>

#endif

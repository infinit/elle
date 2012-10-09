#ifndef ELLE_NETWORK_HOST_HH
# define ELLE_NETWORK_HOST_HH

# include <vector>

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/serialize/fwd.hh>

# include <elle/serialize/fwd.hh>

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
# include <elle/idiom/Open.hh>
      declare(Host);
      Boolean           operator==(const Host&) const;
      Boolean           operator<(const Host&) const;
      Boolean           operator>(const Host&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
    private:
      Type              type;
      String            location;
      ELLE_SERIALIZE_FRIEND_FOR(Host);
    };

  }
}

#include <elle/network/Host.hxx>

#endif

#ifndef ELLE_NETWORK_HEADER_HH
# define ELLE_NETWORK_HEADER_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/network/Tag.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this class represents the header which is attached to every
    /// network message.
    ///
    class Header:
      public radix::Object
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

      Status Create(Tag const);

      //
      // interfaces
      //

      // object
      declare(Header);

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Tag               tag;
    };
  }
}

#include <elle/network/Header.hxx>

#endif

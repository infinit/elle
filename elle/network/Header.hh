#ifndef ELLE_NETWORK_HEADER_HH
# define ELLE_NETWORK_HEADER_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/concurrency/Event.hh>

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

      //
      // methods
      //
      Status            Create(const concurrency::Event&,
                               const Tag,
                               const Natural32);

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
      concurrency::Event             event;
      Tag               tag;
      Natural32         size;
    };

  }
}

#include <elle/network/Header.hxx>

#endif

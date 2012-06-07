#ifndef ELLE_NETWORK_HEADER_HH
# define ELLE_NETWORK_HEADER_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/concurrency/Event.hh>

# include <elle/network/Tag.hh>

# include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;
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
      //Status            Serialize(Archive&) const;
      //Status            Extract(Archive&);

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
#include <elle/network/Header.hxx>

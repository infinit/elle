//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.hh
//
// created       julien quintard   [thu oct 15 14:32:58 2009]
// updated       julien quintard   [mon jul 11 14:41:06 2011]
//

#ifndef ELLE_NETWORK_NETWORK_HH
#define ELLE_NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Trait.hh>

#include <elle/package/Archive.hh>

#include <elle/network/Tag.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Message.hh>

#include <elle/utility/Registrar.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;
  using namespace utility;

  ///
  /// XXX
  ///
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Network
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <const Tag G>
      static Status	Register(
			  const Callback<
			    typename Trait::Constant<
			      typename Message<G>::P
			      >::Type
			    >&);
      static Status	Dispatch(Parcel*);

      static Status	Show(const Natural32 = 0);

      //
      // static attributes
      //
      static Registrar<Tag>	Bureau;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Network.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Address.hh>
#include <elle/network/Bridge.hh>
#include <elle/network/Data.hh>
#include <elle/network/Door.hh>
#include <elle/network/Gate.hh>
#include <elle/network/Header.hh>
#include <elle/network/Host.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Lane.hh>
#include <elle/network/Message.hh>
#include <elle/network/Outputs.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Port.hh>
#include <elle/network/Range.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Session.hh>
#include <elle/network/Slot.hh>
#include <elle/network/Socket.hh>
#include <elle/network/Tag.hh>

#endif

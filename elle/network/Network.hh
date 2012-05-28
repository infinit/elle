//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu oct 15 14:32:58 2009]
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
#include <elle/network/Procedure.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;
  using namespace utility;

  ///
  /// this namespace contains everything related to network communications.
  ///
  /// note that the basic idea behind the Elle library is to define messages
  /// and to register a callback associated with the message's tag of interest
  /// so that such a message being received automatically triggers the
  /// associated callback.
  ///
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables one to register the procedures associated with
    /// message tags so that whenever such a message is received, the
    /// procedure is triggered for handling it.
    ///
    /// this way, the programmer only has to implement functions/methods
    /// for handling messages while the serializing/extracting and calling
    /// is performed automatically by the network manager.
    ///
    class Network
    {
    public:
      //
      // types
      //
      typedef boost::function<elle::Status (TCPSocket*, Locus&, Parcel&)>
      Function;
      typedef std::map<const Tag, Function> Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();

      template <const Tag I,
                const Tag O,
                const Tag E>
      static Status     Register(const Procedure<I, O, E>&);
      static Status     Register(Tag i, const Function& f);

      static Status     Show(const Natural32 = 0);

      //
      // static attributes
      //
      static Container  Procedures;
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

#include <elle/network/Data.hh>
#include <elle/network/Header.hh>
#include <elle/network/Host.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Locus.hh>
#include <elle/network/Message.hh>
#include <elle/network/Outputs.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Port.hh>
#include <elle/network/Procedure.hh>
#include <elle/network/Range.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Session.hh>
#include <elle/network/Tag.hh>
#include <elle/network/TCPSocket.hh>

#endif

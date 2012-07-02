#ifndef ELLE_NETWORK_NETWORK_HH
# define ELLE_NETWORK_NETWORK_HH

#include <elle/types.hh>

#include <elle/network/fwd.hh>
#include <elle/network/Tag.hh>
#include <elle/network/Parcel.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
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

#include <elle/network/Network.hxx>

#endif

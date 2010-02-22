//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Message.hh
//
// created       julien quintard   [wed feb  3 22:01:47 2010]
// updated       julien quintard   [mon feb 22 14:43:14 2010]
//

#ifndef ELLE_NETWORK_MESSAGE_HH
#define ELLE_NETWORK_MESSAGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/archive/Archive.hh>

#include <elle/network/Tag.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Parameters.hh>

namespace elle
{
  using namespace io;
  using namespace archive;

  namespace network
  {

    ///
    /// this class represents a message along with the parameters that
    /// compose it.
    ///
    template <const Tag G>
    class Message:
      public Parameters<>
    {
    public:
      /*
      //
      // identifier
      //
      static const String	Name;

      //
      // methods
      //
      static Status	Pack(Packet&);

      template <const Tag G,
		typename T1>
      static Status	Pack(const T1&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2>
      static Status	Pack(const T1&,
			     const T2&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2,
		typename T3>
      static Status	Pack(const T1&,
			     const T2&,
			     const T3&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2,
		typename T3,
		typename T4>
      static Status	Pack(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      static Status	Pack(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      static Status	Pack(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      static Status	Pack(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      static Status	Pack(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     Packet&);

      template <const Tag G,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      static Status	Pack(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     const T9&,
			     Packet&);
      */
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Message.hxx>

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/channel/Channel.hh
//
// created       julien quintard   [thu oct 15 14:32:58 2009]
// updated       julien quintard   [fri oct 16 05:34:07 2009]
//

#ifndef ELLE_CHANNEL_CHANNEL_HH
#define ELLE_CHANNEL_CHANNEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

#include <elle/channel/Location.hh>
#include <elle/channel/Socket.hh>

namespace elle
{
  using namespace core;
  using namespace archive;

  namespace channel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a channel i.e an abstract communication
    /// object for sending and receiving messages both locally and remotely.
    ///
    class Channel:
      public Dumpable
    {
    public:
      //
      // enumerations
      //
      enum Options
	{
	  OptionNone,
	  OptionAuthentication,
	  OptionEncryption,
	  OptionAll = OptionAuthentication | OptionEncryption
	};

      //
      // methods
      //
      virtual Status	Open(const Options&);
      virtual Status	Close();

      virtual Status	Restrict(const Location&);
      virtual Status	Connect(const Socket&);

      virtual Status	Send(const Archive&,
			     const Location& = Location::Null);
      virtual Status	Receive(Archive&,
				Location& = Location::Null);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/channel/Edge.hh>
#include <elle/channel/Bridge.hh>

#endif

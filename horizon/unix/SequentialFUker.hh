//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:33:15 2011]
//

#ifndef FACADE_UNIX_SEQUENTIALFUKER_HH
#define FACADE_UNIX_SEQUENTIALFUKER_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION		26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <facade/unix/FUker.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <fuse/fuse.h>
# include <fuse/fuse_lowlevel.h>
#include <elle/idiom/Open.hh>

namespace facade
{
  namespace unix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this fuker is an improvement of the interlaced fuker in terms of
    /// safety.
    ///
    /// in order to prevent multiple events to be triggered in parallel,
    /// potentially leading FUSE to believe the first event returned though
    /// it is the second, this fuker orders the events as they come but
    /// triggers them sequentially.
    ///
    /// note that although this technique ensures safety, it impacts the
    /// performance, especially since Infinit is capable of handling concurrent
    /// requests.
    ///
    class SequentialFUker:
      public FUker
    {
    public:
      //
      // enumerations
      //
      enum State
	{
	  StateWaiting,
	  StateProcessing
	};

      //
      // structures
      //
      struct Item
      {
	char*		buffer;
	int		res;
      };

      //
      // types
      //
      typedef std::list<Item>				Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // constructors & destructors
      //
      SequentialFUker();
      ~SequentialFUker();

      //
      // interfaces
      //

      // fuker
      elle::Status	Setup();

      //
      // callbacks
      //
      elle::Status	Event(elle::Natural16);

      //
      // attributes
      //
      elle::Broker*		broker;

      struct ::fuse*		fuse;
      char*			mountpoint;
      struct ::fuse_session*	session;
      struct ::fuse_chan*	channel;
      size_t			size;
      Container			container;

      State			state;
    };

  }
}

#endif

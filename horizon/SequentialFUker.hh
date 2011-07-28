//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/SequentialFUker.hh
//
// created       julien quintard   [tue jul 26 15:33:15 2011]
// updated       julien quintard   [tue jul 26 23:35:52 2011]
//

#ifndef PIG_SEQUENTIALFUKER_HH
#define PIG_SEQUENTIALFUKER_HH

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

#include <pig/FUker.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <fuse/fuse.h>
# include <fuse/fuse_lowlevel.h>
#include <elle/idiom/Open.hh>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
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
    typedef std::list<Item>			Container;
    typedef typename Container::iterator	Iterator;
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
    elle::Status	Event(const elle::Natural16&);

    //
    // attributes
    //
    elle::Broker*		broker;

    struct ::fuse*		fuse;
    char*			mountpoint;
    struct ::fuse_session*	session;
    struct ::fuse_chan*		channel;
    size_t			size;
    Container			container;

    State			state;
  };

}

#endif

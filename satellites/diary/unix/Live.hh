//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [fri jul  1 11:39:35 2011]
//

#ifndef DIARY_UNIX_LIVE_HH
#define DIARY_UNIX_LIVE_HH

//
// ---------- includes --------------------------------------------------------
//
#include <list>

#include <elle/types.hh>

namespace satellite
{
  #undef unix
  namespace unix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// FUSE associates identifiers in order to easily retrieve a context.
    ///
    /// this identifier can be found in the _info_ structure through the
    /// _fh_ i.e file handle attribute.
    ///
    /// since this information depends on the execution, i.e are dynamically
    /// attributed, the replaying system must associate the identifiers
    /// assigned by FUSE with the ones by Infinit.
    ///
    /// for instance, for Open() for instance, FUSE may have assigned the
    /// value 53 with the context representing the opened file. later, every
    /// call to fgetattr, ftruncate, read, write or even release, provides
    /// the identifier in the _info_ structure so that the system does not
    /// have to re-open the file every time.
    ///
    /// these values are actually recorded when the diary is created. however,
    /// when replayed, these numbers do not mean anything because the
    /// Infinit implementation is now used. besides, this implementation
    /// makes use of the _fh_ attribute his own way. for instance, when
    /// replaying the Open() in question, Infinit may actually store
    /// 3298753222 in the _fh_ attribute.
    ///
    /// however, the next replayed operation, say Release(), is going to
    /// come with its own recorded _fh_ attribute i.e 53. since Infinit
    /// is expecting 3298753222 and gets 53, the system crashed.
    ///
    /// the Live class is used to transcode the _fh_ attributes from the
    /// recorded version to the live ones. taking the example above for
    /// instance, with the call to Release(), the _info_ structure would
    /// be modified so that the _fh_ attribute no longer holds 53 but
    /// 3298753222.
    ///
    class Live
    {
    public:
      //
      // classes
      //
      class Item
      {
      public:
        //
        // constructors & destructors
        //
        Item(const elle::Natural64,
             const elle::Natural64);

        //
        // attributes
        //
        elle::Natural64         passive;
        elle::Natural64         active;
      };

      //
      // types
      //
      typedef std::list<Item*>                          Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

      static elle::Status       Add(const elle::Natural64,
                                    const elle::Natural64);
      static elle::Status       Retrieve(const elle::Natural64,
                                         elle::Natural64&);
      static elle::Status       Remove(const elle::Natural64);

      static elle::Status       Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static Container          Items;
    };

  }
}

#endif

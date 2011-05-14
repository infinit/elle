//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/local/Local.hh
//
// created       julien quintard   [tue apr 13 16:05:04 2010]
// updated       julien quintard   [fri may 13 11:36:54 2011]
//

#ifndef HOLE_LOCAL_LOCAL_HH
#define HOLE_LOCAL_LOCAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Holeable.hh>

namespace hole
{
  namespace local
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the local hole implementation stores data on the local node's
    /// operating system storage.
    ///
    class Local:
      public Holeable
    {
    public:
      //
      // constructors & destructors
      //
      Local(const nucleus::Network&);

      //
      // attributes
      //
      // XXX

      //
      // interfaces
      //

      // holeable
      elle::Status	Join();
      elle::Status	Leave();

      elle::Status	Put(const nucleus::Address&,
			    const nucleus::Block&);
      elle::Status	Get(const nucleus::Address&,
			    nucleus::Block&);
      elle::Status	Gather(const nucleus::Address&,
			       nucleus::Block&);
      elle::Status	Erase(const nucleus::Address&);
    };

  }
}

#endif

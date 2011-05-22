//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Remote.hh
//
// created       julien quintard   [fri may 20 19:31:08 2011]
// updated       julien quintard   [sun may 22 11:36:22 2011]
//

#ifndef HOLE_LOCAL_REMOTE_HH
#define HOLE_LOCAL_REMOTE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Holeable.hh>

namespace hole
{
  namespace remote
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the remote hole implementation stores data on a remote host's
    /// storage.
    ///
    class Remote:
      public Holeable
    {
    public:
      //
      // constructors & destructors
      //
      Remote(const nucleus::Network&);

      //
      // methods
      //
      elle::Status	Host(const elle::Address&);

      //
      // interfaces
      //

      // holeable
      elle::Status	Join();
      elle::Status	Leave();

      elle::Status	Put(const nucleus::Address&,
			    const nucleus::ImmutableBlock&);
      elle::Status	Put(const nucleus::Address&,
			    const nucleus::MutableBlock&);
      elle::Status	Get(const nucleus::Address&,
			    nucleus::ImmutableBlock&);
      elle::Status	Get(const nucleus::Address&,
			    const nucleus::Version&,
			    nucleus::MutableBlock&);
      elle::Status	Kill(const nucleus::Address&);

      //
      // attributes
      //
      elle::Address	host;
    };

  }
}

#endif

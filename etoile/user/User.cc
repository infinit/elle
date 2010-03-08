//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/User.cc
//
// created       julien quintard   [thu mar  4 12:39:12 2010]
// updated       julien quintard   [thu mar  4 16:51:39 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/User.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the authenticated users connected to
    /// etoile and therefore allowed to perform requests.
    ///
    User::Data::Container	User::Bank;

    ///
    /// this container holds the connections received but non-authenticated
    /// yet.
    ///
    User::Temporary::Container	User::Queue;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds a door to the queue.
    ///
    Status		User::Record(Door*			door)
    {
      enter();

      // simply add the door to the queue.
      //User::Queue.push_front(User::Temporary::Value(type, door));

      leave();
    }

  }
}

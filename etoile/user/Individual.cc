//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Individual.cc
//
// created       julien quintard   [thu mar  4 12:22:16 2010]
// updated       julien quintard   [thu mar  4 12:24:23 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Individual.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this constructor initializes the individual.
    ///
    Individual::Individual():
      status(Individual::StatusUnknown),
      client(NULL),
      agent(NULL)
    {
    }

  }
}

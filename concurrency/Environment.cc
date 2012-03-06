//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu apr  8 15:47:45 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Environment.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the environment.
    ///
    Status              Environment::Dump(const Natural32       margin) const
    {
      String                    alignment(margin, ' ');
      Environment::Scoutor      scoutor;

      std::cout << alignment << "[Environment] "
                << this->container.size() << std::endl;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          std::cout << alignment << Dumpable::Shift << scoutor->first
                    << " :: " << scoutor->second << std::endl;
        }

      return StatusOk;
    }

  }
}

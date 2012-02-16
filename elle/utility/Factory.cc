//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu jan 28 19:19:35 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Factory.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Factory::~Factory()
    {
      // clear the factory.
      this->Clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method clears the factory registered products..
    //
    Status              Factory::Clear()
    {
      Factory::Scoutor  scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          // delete the functionoid.
          delete scoutor->second;
        }

      // clear the container.
      this->container.clear();

      return elle::StatusOk;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the factory.
    ///
    Status              Factory::Dump(const Natural32           margin) const
    {
      String            alignment(margin, ' ');
      Factory::Scoutor  scoutor;

      std::cout << alignment << "[Factory]" << std::endl;

      // go through the map.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          std::cout << alignment << Dumpable::Shift
                    << scoutor->first << std::endl;
        }

      return elle::StatusOk;
    }

  }
}

#include <elle/utility/Factory.hh>

#include <elle/idiom/Close.hh>
# include <iostream>
#include <elle/idiom/Open.hh>

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

      return Status::Ok;
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
          std::cout << alignment << io::Dumpable::Shift
                    << scoutor->first << std::endl;
        }

      return Status::Ok;
    }

  }
}

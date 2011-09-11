//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/shrub/Group.cc
//
// created       julien quintard   [sat sep 10 16:55:16 2011]
// updated       julien quintard   [sun sep 11 11:34:38 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/shrub/Group.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds a riffle to the group.
    ///
    elle::Status	Group::Add(Riffle*			riffle)
    {
      Group::Iterator	iterator;

      enter();

      // add the riffle to the container.
      this->container.push_back(riffle);

      leave();
    }

    ///
    /// this method removes a riffle from the group.
    ///
    elle::Status	Group::Remove(Riffle*			riffle)
    {
      Group::Iterator	iterator;

      enter();

      // go through the riffles.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  // check if this riffle is the one we are looking for.
	  if (*iterator == riffle)
	    {
	      // remove the riffle from the container.
	      this->container.erase(iterator);

	      break;
	    }
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the group.
    ///
    elle::Status	Group::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');
      Group::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Group] "
		<< std::dec << this->container.size() << std::endl;

      // go through the group.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  // display the riffle.
	  std::cout << alignment
		    << elle::Dumpable::Shift
		    << "[Riffle] " << std::hex << *scoutor << std::endl;
	}

      leave();
    }

  }
}

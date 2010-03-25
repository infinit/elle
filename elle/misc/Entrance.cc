//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Entrance.cc
//
// created       julien quintard   [wed mar 24 15:51:36 2010]
// updated       julien quintard   [thu mar 25 00:54:37 2010]
//

#ifndef ELLE_MISC_ENTRANCE_HXX
#define ELLE_MISC_ENTRANCE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Entrance.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts
///
#include <elle/misc/Maid.hh>
#include <elle/misc/Report.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Entrance::Entrance():
      routine(NULL)
    {
    }

    ///
    /// routine constructor.
    ///
    Entrance::Entrance(const Routine&			routine):
      routine(NULL)
    {
      enter();

      // clone the routine.
      if (routine.Clone((Entity*&)this->routine) == StatusError)
	alert("unable to clone the routine");

      release();
    }

    ///
    /// copy constructor.
    ///
    Entrance::Entrance(const Entrance&			entrance)
    {
      enter();

      // delete the previous routine.
      if (this->routine != NULL)
	delete this->routine;

      // allocate and copy the routine, if necessary.
      if (entrance.routine != NULL)
	{
	  // clone the routine.
	  if (entrance.routine->Clone((Entity*&)this->routine) == StatusError)
	    alert("unable to clone the routine");
	}
      else
	{
	  // set the pointer to NULL.
	  this->routine = NULL;
	}

      release();
    }

    ///
    /// destructor.
    ///
    Entrance::~Entrance()
    {
      // release the routine.
      if (this->routine != NULL)
	delete this->routine;
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Entrance);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the entrance.
    ///
    Status		Entrance::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Entrance]" << std::endl;

      // dump the routine.
      if (this->routine != NULL)
	if (this->routine->Dump(margin + 2) == StatusError)
	  escape("unable to dump the routine");

      leave();
    }

  }
}

#endif

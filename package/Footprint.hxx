//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author           [sat sep 24 23:35:29 2011]
//

#ifndef ELLE_PACKAGE_FOOTPRINT_HXX
#define ELLE_PACKAGE_FOOTPRINT_HXX

namespace elle
{
  namespace package
  {

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method recycles a footprint.
    ///
    template <typename T>
    Status		Footprint::Recycle(const T*		object)
    {
      // release the resources.
      this->~Footprint();

      if (object == NULL)
	{
	  // initialize the object with default values.
	  new (this) T;
	}
      else
	{
	  // initialize the object with defined values.
	  new (this) T(*object);
	}

      // return StatusOk in order to avoid including Report, Status and Maid.
      return (StatusOk);
    }

  }
}

#endif

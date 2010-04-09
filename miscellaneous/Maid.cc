//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Maid.cc
//
// created       julien quintard   [sat mar 20 00:10:35 2010]
// updated       julien quintard   [sat mar 20 15:28:56 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Maid.hh>

///
/// these includes are placed here to prevent pre-processing conflicts.
///
#include <elle/core/String.hh>

#include <elle/miscellaneous/Report.hh>

namespace elle
{
  namespace miscellaneous
  {

//
// ---------- garrison --------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    Maid::Garrison::Garrison(Natural32				size,
			     ...):
      size(size)
    {
      va_list		ap;
      Natural32		i;

      // check the size.
      if (this->size > Maid::Garrison::Capacity)
	fail("unable to store more guards than the garrison's capacity");

      // start the variadic extraction.
      va_start(ap, size);

      // read the arguments and set the guards.
      for (i = 0; i < size; i++)
	this->guards[i] = va_arg(ap, Maid::Guard*);

      // stop the extraction.
      va_end(ap);
    }

    ///
    /// destructor.
    ///
    Maid::Garrison::~Garrison()
    {
      Natural32		i;

      // go through all the guards.
      for (i = 0; i < this->size; i++)
	delete this->guards[i];
    }

  }
}

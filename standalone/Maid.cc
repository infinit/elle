//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Maid.cc
//
// created       julien quintard   [sat mar 20 00:10:35 2010]
// updated       julien quintard   [sun may  2 15:21:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/core/Character.hh>

#include <elle/core/String.hh>

namespace elle
{
  namespace standalone
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

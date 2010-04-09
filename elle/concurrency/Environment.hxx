//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Environment.hxx
//
// created       julien quintard   [thu apr  8 14:18:10 2010]
// updated       julien quintard   [thu apr  8 16:03:43 2010]
//

#ifndef ELLE_CONCURRENCY_ENVIRONMENT_HXX
#define ELLE_CONCURRENCY_ENVIRONMENT_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Report.hh>
#include <elle/miscellaneous/Maid.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method stores an element.
    ///
    template <typename T>
    Status		Environment::Store(const String&	name,
					   T*			value)
    {
      enter();

      // push the (name, value) tuple.
      this->container.push_back(Environment::Value(name, (Void*)value));

      leave();
    }

    ///
    /// this method loads a value.
    ///
    template <typename T>
    Status		Environment::Load(const String&		name,
					  T*&			value)
    {
      Environment::Iterator	iterator;

      enter();

      // go through the elements.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  // check if this is the looked for element.
	  if (iterator->first == name)
	    {
	      // return the value.
	      value = (T*)iterator->second;

	      // remove the item
	      this->container.erase(iterator);

	      leave();
	    }
	}

      escape("unable to locate the element");
    }

  }
}

#endif

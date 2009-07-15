//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/test/archive/Referee.cc
//
// created       julien quintard   [wed jan 28 12:32:31 2009]
// updated       julien quintard   [fri feb  6 21:03:49 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Referee.hh"

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace test
  {

//
// ---------- definitions -----------------------------------------------------
//

    std::list<Element*>			Referee::list;

//
// ---------- methods ---------------------------------------------------------
//

    Status		Referee::Push(const Archive::Type	type,
				      const void*		data,
				      const Natural32		size)
    {
      Element*		element = new Element;

      element->type = type;

      if ((element->data = ::malloc(size)) == NULL)
	escape("unable to allocate memory");

      ::memcpy(element->data, data, size);

      element->size = size;

      Referee::list.push_back(element);

      leave();
    }

    Status		Referee::Pop(Archive::Type&		type,
				     void*&			data,
				     Natural32&			size)
    {
      Element*		element;

      if (Referee::list.empty() == true)
	escape("the referee does not contain any element any more");

      element = Referee::list.front();

      type = element->type;
      data = element->data;
      size = element->size;

      Referee::list.pop_front();

      leave();
    }

  }
}

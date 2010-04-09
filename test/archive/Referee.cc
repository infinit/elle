//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/archive/Referee.cc
//
// created       julien quintard   [wed jan 28 12:32:31 2009]
// updated       julien quintard   [wed mar  3 11:06:24 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Referee.hh"

namespace elle
{
  using namespace core;
  using namespace miscellaneous;
  using namespace archive;

  namespace test
  {

//
// ---------- definitions -----------------------------------------------------
//

    std::list<Element*>			Referee::List;

//
// ---------- methods ---------------------------------------------------------
//

    Status		Referee::Push(const Archive::Type	type,
				      const void*		data,
				      const Natural32		size)
    {
      Element*		element;

      enter(instance(element));

      element = new Element;

      element->type = type;

      if ((element->data = ::malloc(size)) == NULL)
	escape("unable to allocate memory");

      ::memcpy(element->data, data, size);

      element->size = size;

      Referee::List.push_back(element);

      waive(element);

      leave();
    }

    Status		Referee::Pop(Archive::Type&		type,
				     void*&			data,
				     Natural32&			size)
    {
      Element*		element;

      enter();

      if (Referee::List.empty() == true)
	escape("the referee does not contain any element any more");

      element = Referee::List.front();

      type = element->type;
      data = element->data;
      size = element->size;

      Referee::List.pop_front();

      delete element;

      leave();
    }

    Status		Referee::Flush()
    {
      Element*		element;

      enter();

      while (Referee::List.empty() == false)
	{
	  element = Referee::List.front();

	  ::free(element->data);

	  Referee::List.pop_front();

	  delete element;
	}

      leave();
    }

  }
}

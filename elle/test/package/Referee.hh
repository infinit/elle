//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/package/Referee.hh
//
// created       julien quintard   [tue jan 27 16:56:46 2009]
// updated       julien quintard   [fri jun  3 15:08:16 2011]
//

#ifndef ELLE_TEST_PACKAGE_REFEREE_HH
#define ELLE_TEST_PACKAGE_REFEREE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <list>

namespace elle
{
  namespace test
  {

//
// ---------- structures ------------------------------------------------------
//

    struct Element
    {
      Archive::Type	type;
      void*		data;
      Natural32		size;
    };

//
// ---------- classes ---------------------------------------------------------
//

    class Referee
    {
    public:
      //
      // methods
      //
      static Status	Push(const Archive::Type,
			     const void*,
			     const Natural32);
      static Status	Pop(Archive::Type&,
			    void*&,
			    Natural32&);

      static Status	Flush();

      //
      // attributes
      //
      static std::list<Element*>	List;
    };

  }
}

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/archive/Referee.hh
//
// created       julien quintard   [tue jan 27 16:56:46 2009]
// updated       julien quintard   [wed mar  3 11:05:44 2010]
//

#ifndef ELLE_TEST_ARCHIVE_REFEREE_HH
#define ELLE_TEST_ARCHIVE_REFEREE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

#include <list>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

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

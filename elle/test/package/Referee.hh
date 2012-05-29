//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue jan 27 16:56:46 2009]
//

#ifndef ELLE_TEST_PACKAGE_REFEREE_HH
#define ELLE_TEST_PACKAGE_REFEREE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

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
      Archive::Type     type;
      void*             data;
      Natural32         size;
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
      static Status     Push(const Archive::Type,
                             const void*,
                             const Natural32);
      static Status     Pop(Archive::Type&,
                            void*&,
                            Natural32&);

      static Status     Flush();

      //
      // attributes
      //
      static std::list<Element*>        List;
    };

  }
}

#endif

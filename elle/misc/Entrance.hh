//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Entrance.hh
//
// created       julien quintard   [wed mar 24 15:49:05 2010]
// updated       julien quintard   [thu mar 25 00:28:32 2010]
//

#ifndef ELLE_MISC_ENTRANCE_HH
#define ELLE_MISC_ENTRANCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Status.hh>
#include <elle/misc/Routine.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an entrance i.e a pointer-based routine.
    ///
    class Entrance:
      public Entity,
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Entrance();
      Entrance(const Routine&);
      Entrance(const Entrance&);
      ~Entrance();

      //
      // methods
      //
      template <typename... T>
      Status		Trigger(T*...);

      //
      // interfaces
      //

      // entity
      declare(Entity, Entrance);
      // XXX

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Routine*		routine;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/misc/Entrance.hxx>

#endif

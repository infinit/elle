//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/State.hh
//
// created       julien quintard   [wed mar 31 16:05:57 2010]
// updated       julien quintard   [tue apr  6 12:26:55 2010]
//

#ifndef ETOILE_WALL_STATE_HH
#define ETOILE_WALL_STATE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Genre.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Version.hh>

namespace etoile
{

  namespace kernel
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Object;

  }

  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class describes an object's meta data.
    ///
    class State:
      public Dumpable, public Archivable
    {
    public:
      //
      // method
      //
      Status		Create(const kernel::Object&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      kernel::Genre	genre;

      struct
      {
	Time		creation;
	Time		modification;
      }			stamps;

      kernel::Offset	size;

      struct
      {
	PublicKey	owner;
	PublicKey	author;
      }			Ks;

      struct
      {
	kernel::Version	data;
	kernel::Version	meta;
      }			versions;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Object.hh>
#include <etoile/kernel/Role.hh>

#endif

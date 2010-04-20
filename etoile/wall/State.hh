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
// updated       julien quintard   [sun apr 18 19:12:28 2010]
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
#include <etoile/kernel/Permissions.hh>

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
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const State		Null;

      //
      // method
      //
      Status		Create(const kernel::Object&);

      //
      // interfaces
      //

      // entity
      declare(Entity, State);
      Boolean		operator==(const State&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      kernel::Genre		genre;

      struct
      {
	Time			creation;
	Time			modification;
      }				stamps;

      kernel::Offset		size;

      struct
      {
	PublicKey		owner;
	PublicKey		author;
      }				keys;

      struct
      {
	kernel::Permissions	owner;
      }				permissions;

      struct
      {
	kernel::Version		data;
	kernel::Version		meta;
      }				versions;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Object.hh>
#include <etoile/kernel/Role.hh>

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Status.hh
//
// created       julien quintard   [wed mar 31 16:05:57 2010]
// updated       julien quintard   [fri may 28 17:50:12 2010]
//

#ifndef ETOILE_WALL_STATUS_HH
#define ETOILE_WALL_STATUS_HH

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
    class Status:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Status		Null;

      //
      // method
      //
      elle::Status	Create(const kernel::Object&);

      //
      // interfaces
      //

      // object
      declare(Status);
      elle::Boolean	operator==(const Status&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      kernel::Genre		genre;

      struct
      {
	elle::Time		creation;
	elle::Time		modification;
      }				stamps;

      kernel::Offset		size;

      struct
      {
	elle::PublicKey		owner;
	elle::PublicKey		author;
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

#endif

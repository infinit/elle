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
// updated       julien quintard   [thu may  5 16:31:04 2011]
//

#ifndef ETOILE_WALL_STATUS_HH
#define ETOILE_WALL_STATUS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace etoile
{

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
      // constructors & destructors
      //
      Status();

      //
      // method
      //
      elle::Status	Create(const nucleus::Object&);

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
      nucleus::Genre		genre;

      struct
      {
	elle::Time		creation;
	elle::Time		modification;
      }				stamps;

      nucleus::Offset		size;

      struct
      {
	elle::PublicKey		owner;
	elle::PublicKey		author;
      }				keys;

      struct
      {
	nucleus::Permissions	owner;
      }				permissions;

      struct
      {
	nucleus::Version	data;
	nucleus::Version	meta;
      }				versions;
    };

  }
}

#endif

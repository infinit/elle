//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/miscellaneous/Information.hh
//
// created       julien quintard   [wed mar 31 16:05:57 2010]
// updated       julien quintard   [tue jun 14 13:50:41 2011]
//

#ifndef ETOILE_MISCELLANEOUS_INFORMATION_HH
#define ETOILE_MISCELLANEOUS_INFORMATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace etoile
{
  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class describes an object's meta data.
    ///
    class Information:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Information		Null;

      //
      // constructors & destructors
      //
      Information();

      //
      // method
      //
      elle::Status	Create(const nucleus::Object&);

      //
      // interfaces
      //

      // object
      declare(Information);
      elle::Boolean	operator==(const Information&) const;

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

//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Subject.hh
//
// created       julien quintard   [fri aug 14 16:26:10 2009]
// updated       julien quintard   [mon aug 17 02:34:03 2009]
//

#ifndef ETOILE_CORE_SUBJECT_HH
#define ETOILE_CORE_SUBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Subject
    {
    public:
      //
      // enumeration
      //
      enum Type
	{
	  TypeUnknown,
	  TypeUser,
	  TypeGroup
	};

      //
      // constructors & destructors
      //
      Subject();
      ~Subject();

      //
      // methods
      //
      Status		Create(const PublicKey&);
      Status		Create(const hole::Address&);

      //
      // attributes
      //
      Type		type;

      union
      {
	PublicKey*	user;
	hole::Address*	group;
      }			identifier;

      // XXX dumpable, archivable
    };

  }
}

#endif

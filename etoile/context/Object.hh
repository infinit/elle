//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Object.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [mon aug 17 12:26:46 2009]
//

#ifndef ETOILE_CONTEXT_OBJECT_HH
#define ETOILE_CONTEXT_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>
#include <etoile/context/Context.hh>

#include <list>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    /// when the object is loaded or created, the permissions on the
    /// object for the subject are recorded in the permissions field.
    ///
    class Object:
      public Context
    {
    public:
      //
      // enumerations
      //
      enum Dirty
	{
	  DirtyNone,
	  DirtyData,
	  DirtyMeta,
	  DirtyObject
	};

      //
      // constructors & destructors
      Object();
      Object(const Object&);
      ~Object();

      //
      // attributes
      //
      Dirty				dirty;

      core::Permissions			rights;

      // XXX path::Route		route;

      // XXX branler un constructeur/destructeur

      hole::Address			address;

      core::Object			object;

      union
      {
	core::Data*			data;
	core::Catalog*			catalog;
	core::Reference*		reference;
      }					contents;

      core::Access*			access;

      // XXX ajouter un transcript avec toutes les operations affectuees
      //  a partir du moment ou l'object a ete loade au moment ou il a ete
      //  store. de cette maniere, dans le cas d'un probleme de version
      //  (l'objet a ete modifie en parallele), on peut re-run les operations
      //  sur la nouvelle version si le user le veut.
    };

  }
}

#endif

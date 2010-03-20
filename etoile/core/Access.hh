//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Access.hh
//
// created       julien quintard   [thu mar  5 20:17:45 2009]
// updated       julien quintard   [sat mar 20 04:11:58 2010]
//

#ifndef ETOILE_CORE_ACCESS_HH
#define ETOILE_CORE_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>

#include <etoile/core/ContentHashBlock.hh>
#include <etoile/core/Permissions.hh>
#include <etoile/core/Subject.hh>
#include <etoile/core/State.hh>
#include <etoile/core/Offset.hh>

#include <list>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Access:
      public ContentHashBlock
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeUser,
	  TypeGroup
	};

      //
      // structures
      //
      struct		Entry
      {
	Subject		subject;
	Permissions	permissions;
	Code		token;
      };

      //
      // types
      //
      typedef std::list<Entry*>		Container;
      typedef Container::iterator	Iterator;
      typedef Container::const_iterator	Scoutor;

      //
      // methods
      //
      Status		Add(const Subject&,
			    const Permissions&,
			    const Code&);
      Status		Update(const Subject&,
			       const Permissions&,
			       const Code&);
      Status		Remove(const Subject&);
      Status		Retrieve(const Subject&,
				 Permissions&);
      Status		Retrieve(const Subject&,
				 const PrivateKey&,
				 Code&);

      Status		Search(const Subject&,
			       Access::Iterator* = NULL);

      Status		Size(Offset&) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Access);
      // XXX operator==

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      State		state;

      Container		entries;
    };

  }
}

#endif

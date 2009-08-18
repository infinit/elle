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
// updated       julien quintard   [mon aug 17 01:48:33 2009]
//

#ifndef ETOILE_CORE_ACCESS_HH
#define ETOILE_CORE_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/ContentHashBlock.hh>
#include <etoile/core/Permissions.hh>

#include <etoile/hole/Hole.hh>

#include <vector>

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
	Type		type;
      };

      struct		User:
	public Entry
      {
	PublicKey	K;
	Permissions	permissions;
      };

      struct		Group:
	public Entry
      {
	hole::Address	descriptor;
	Permissions	permissions;

	hole::Address	members;
      };

      //
      // constructors & destructors
      //

      // XXX

      //
      // methods
      //
      Status		Locate(const PublicKey&,
			       Entry*);
      Status		Locate(const hole::Address&,
			       Entry*);

      Status		Add();
      Status		Remove();

      //
      // attributes
      //
      std::vector<Entry*>	entries;

      //
      // interfaces
      //

      // XXX
    };

  }
}

#endif

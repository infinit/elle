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
// updated       julien quintard   [thu jul 30 19:52:57 2009]
//

#ifndef ETOILE_COMPONENTS_ACCESS_HH
#define ETOILE_COMPONENTS_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/ContentHashBlock.hh>
#include <etoile/core/Permissions.hh>

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
	Address		descriptor;
	Permissions	permissions;

	Address		members;
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
      Status		Locate(const Address&,
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

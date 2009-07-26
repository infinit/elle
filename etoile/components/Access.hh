//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Access.hh
//
// created       julien quintard   [thu mar  5 20:17:45 2009]
// updated       julien quintard   [fri jul 24 16:31:59 2009]
//

#ifndef ETOILE_COMPONENTS_ACCESS_HH
#define ETOILE_COMPONENTS_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/ContentHashBlock.hh>
#include <etoile/components/Permissions.hh>

#include <vector>

namespace etoile
{
  namespace components
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
      // constants
      //
      static const String		Class;

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

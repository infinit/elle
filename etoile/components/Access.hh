//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       GPL
//
// file          /home/mycure/infinit/infinit/components/Access.hh
//
// created       julien quintard   [thu mar  5 20:17:45 2009]
// updated       julien quintard   [wed mar 11 16:57:05 2009]
//

#ifndef INFINIT_COMPONENTS_ACCESS_HH
#define INFINIT_COMPONENTS_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <infinit/components/ContentHashBlock.hh>

#include <vector>

namespace infinit
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
      enum Rights
	{
	  RightNone = 0,
	  RightRead = 1,
	  RightWrite = 2,
	};

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
	Rights		rights;
      };

      struct		Group:
	public Entry
      {
	Address		descriptor;
	Rights		rights;

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

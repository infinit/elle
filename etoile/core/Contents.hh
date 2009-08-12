//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Contents.hh
//
// created       julien quintard   [mon aug 10 12:07:15 2009]
// updated       julien quintard   [tue aug 11 00:45:54 2009]
//

#ifndef ETOILE_CORE_CONTENTS_HH
#define ETOILE_CORE_CONTENTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Address.hh>
#include <etoile/core/Component.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this classes abstracts the data contained in an object.
    ///
    /// indeed, in some cases, the data is so small that it fits in a single
    /// blocks. therefore, there is no need for an advanced data structure
    /// such as a tree. however, in other cases, a simple data structure such
    /// as a list would be too slow to handle a lot of data.
    ///
    /// this class enables objects to use both models according to the
    /// actual data size. thus, when the data no longer fits in a single block
    /// a more flexible data structure is created.
    ///
    class Contents:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeNone = 0,
	  TypeRaw,
	  TypeSet
	};

      //
      // methods
      //
      Status		Create(const Type);

      //
      // interfaces
      //

      // entity
      Contents&		operator=(const Contents&);
      Boolean		operator==(const Contents&) const;
      Boolean		operator!=(const Contents&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Type		type;
      Address		address;
      Natural32		size;
    };

  }
}

#endif

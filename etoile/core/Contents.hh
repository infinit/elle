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
// updated       julien quintard   [mon aug 17 02:05:44 2009]
//

#ifndef ETOILE_CORE_CONTENTS_HH
#define ETOILE_CORE_CONTENTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>

#include <etoile/core/ContentHashBlock.hh>

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
    class Contents:
      public ContentHashBlock
    {
    public:
      //
      // constructors & destructors
      //
      Contents();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      hole::Address	address;
      Natural32		size;
    };

  }
}

#endif

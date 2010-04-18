//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/miscellaneous/Region.hh
//
// created       julien quintard   [mon nov 12 23:19:13 2007]
// updated       julien quintard   [sun apr 18 15:00:32 2010]
//

#ifndef ELLE_MISCELLANEOUS_REGION_HH
#define ELLE_MISCELLANEOUS_REGION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Entity.hh>
#include <elle/core/Natural.hh>

#include <elle/io/Dumpable.hh>

#include <elle/miscellaneous/Byte.hh>
#include <elle/miscellaneous/Status.hh>

#include <elle/idiom/Close.hh>
# include <iostream>
# include <ios>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace miscellaneous;

  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a memory area.
    ///
    /// note that this class is *not* archivable!
    ///
    class Region:
      public Entity,
      public Dumpable
    {
    public:
      //
      // enumerations
      //

      ///
      /// a chunk represents a memory area that already exists and that is
      /// wrapped into a so-called chunk. a buffer represents an allocated
      /// and managed memory area.
      ///
      enum Type
	{
	  TypeUnknown = 0,
	  TypeChunk,
	  TypeBuffer
	};

      ///
      /// the detach option, activated through Detach(), guarantee
      /// that the data will not be released when the object is destroyed.
      ///
      enum Options
	{
	  OptionNone = 0,
	  OptionDetach
	};

      //
      // constructors & destructors
      //
      Region();
      Region(const Region&);
      ~Region();

      //
      // methods
      //
      Status		Wrap(Byte*,
			     Natural64);
      Status		Acquire(Byte*,
				Natural64);

      Status		Prepare(const Natural64);
      Status		Duplicate(Byte*,
				  Natural64);

      Status		Adjust(const Natural64);
      Status		Append(const Byte*,
			       const Natural64);

      Status		Read(const Natural64,
			     Byte*,
			     const Natural64) const;
      Status		Write(const Natural64,
			      const Byte*,
			      const Natural64);

      Status		Detach();

      //
      // interfaces
      //

      // entity
      declare(Entity, Region);
      Boolean		operator==(const Region&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type		type;
      Options		options;

      Byte*		contents;
      Natural64		size;
      Natural64		capacity;
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const elle::miscellaneous::Region&);
}

#endif

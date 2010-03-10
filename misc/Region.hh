//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/misc/Region.hh
//
// created       julien quintard   [mon nov 12 23:19:13 2007]
// updated       julien quintard   [wed mar 10 20:33:17 2010]
//

#ifndef ELLE_MISC_REGION_HH
#define ELLE_MISC_REGION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/io/IO.hh>

#include <elle/misc/Byte.hh>
#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>

#include <elle/idiom/Close.hh>
# include <ios>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace misc;

  namespace misc
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
			     Natural32);
      Status		Acquire(Byte*,
				Natural32);

      Status		Prepare(const Natural32);
      Status		Duplicate(Byte*,
				  Natural32);

      Status		Adjust(const Natural32);
      Status		Append(const Byte*,
			       const Natural32);

      Status		Read(const Natural32,
			     Byte*,
			     const Natural32) const;
      Status		Write(const Natural32,
			      const Byte*,
			      const Natural32);

      Status		Detach();

      //
      // interfaces
      //

      // entity
      embed(Entity, Region);
      Boolean		operator==(const Region&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Type		type;
      Options		options;

      Byte*		contents;
      Natural32		size;
      Natural32		capacity;
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const elle::misc::Region&);
}

#endif

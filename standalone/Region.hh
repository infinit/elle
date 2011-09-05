//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Region.hh
//
// created       julien quintard   [mon nov 12 23:19:13 2007]
// updated       julien quintard   [sat sep  3 20:32:01 2011]
//

#ifndef ELLE_STANDALONE_REGION_HH
#define ELLE_STANDALONE_REGION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Byte.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Meta.hh>

#include <elle/io/Dumpable.hh>

#include <elle/idiom/Close.hh>
# include <iostream>
# include <iomanip>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace io;

  namespace standalone
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a memory area.
    ///
    /// note that this class is *not* archivable in order to prevent
    /// conflicts, hence cannot derive the Object class.
    ///
    /// therefore the operators must be manually defined for this class
    /// along with fundamental methods such as Recycle() etc.
    ///
    class Region:
      public Meta,
      public virtual Dumpable
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
      Region(Byte*,
	     Natural64);
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

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // object-like
      template <typename T>
      Status		Recycle(const T* = NULL);

      virtual Status	Imprint(Natural32&) const;
      virtual Status	Clone(Region*&) const;

      virtual Boolean	operator==(const Region&) const;
      virtual Boolean	operator<(const Region&) const;
      virtual Boolean	operator>(const Region&) const;

      virtual Region&	operator=(const Region&);
      virtual Boolean	operator!=(const Region&) const;
      virtual Boolean	operator<=(const Region&) const;
      virtual Boolean	operator>=(const Region&) const;

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
// ---------- templates -------------------------------------------------------
//

#include <elle/standalone/Region.hxx>

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu nov  1 21:00:41 2007]
//

#ifndef ELLE_PACKAGE_ARCHIVE_HH
#define ELLE_PACKAGE_ARCHIVE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>
#include <elle/core/Integer.hh>
#include <elle/core/Large.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Null.hh>
#include <elle/core/Real.hh>
#include <elle/core/String.hh>
#include <elle/core/Void.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

#include <elle/system/System.hh>

#include <elle/idiom/Close.hh>
# include <stdlib.h>
# include <msgpack.hpp>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace system;

  namespace package
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// the Archivable must be forward declared to prevent conflicts.
    ///
    class Archivable;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides methods for serializing basic data types. each
    /// given data is appended to the archive in its original format.
    ///
    /// note that the archive type are recorded prior to the data although
    /// the underlying serialization format also records the type of the
    /// data being serialized. this choice has been made to distinguish
    /// a character from a string, a region or even an archive as they
    /// all end up being serialized as raw data.
    ///
    /// the Archive class does not derive anything, such as the Object
    /// class. instead, the class implements an object-like interface
    /// in order to prevent conflicts.
    ///
    /// finally, note that in order to provide a coherent interface to
    /// the user, a _contents_ and _size_ attribute are provided, both
    /// referencing the buffer's attributes. besides, whenever the archive
    /// is set up for extraction, the region's content is recorded in
    /// the buffer so that both serialization and extraction buffers lay
    /// at the same place.
    ///
    /// in addition, the _control_ attribute indicates whether the content
    /// set up in the buffer comes from an acquired region and therefore
    /// must be deleted.
    ///
    class Archive
    {
    public:
      //
      // enumerations
      //

      ///
      /// this enumeration enables the archive objects to know if they are
      /// operating a serialization or an extraction.
      ///
      enum Mode
	{
	  ModeUnknown,
	  ModeSerialization,
	  ModeExtraction
	};

      ///
      /// this enumeration indicates whether the archive has taken control
      /// over the region used for extraction.
      ///
      enum Control
	{
	  ControlUnknown,
	  ControlAcquired,
	  ControlWrapped
	};

      ///
      /// this enumeration types every basic element archives are composed of.
      ///
      enum Type
	{
	  TypeUnknown = 0,
	  TypeNull,
	  TypeBoolean,
	  TypeCharacter,
	  TypeReal,
	  TypeInteger8,
	  TypeInteger16,
	  TypeInteger32,
	  TypeInteger64,
	  TypeNatural8,
	  TypeNatural16,
	  TypeNatural32,
	  TypeNatural64,
	  TypeLarge,
	  TypeString,
	  TypeRegion,
	  TypeArchive
	};

      //
      // constructors & destructors
      //
      Archive();
      Archive(const Archive&);
      ~Archive();

      //
      // methods
      //
      Status		Create();

      Status		Acquire(const Region&);
      Status		Wrap(const Region&);

      Status		Serialize();
      template <typename T>
      Status		Serialize(const T&);
      template <typename T,
		typename... TT>
      Status		Serialize(const T&,
				  const TT&...);
      template <typename T>
      Status		Serialize(const T*);
      template <typename T,
		typename... TT>
      Status		Serialize(const T*,
				  const TT*...);

      Status		Extract();
      template <typename T>
      Status		Extract(T&);
      template <typename T,
		typename... TT>
      Status		Extract(T&,
				TT&...);
      template <typename T>
      Status		Extract(T*);
      template <typename T,
		typename... TT>
      Status		Extract(T*,
				TT*...);

      template <typename T>
      Status		Store(const T&);
      Status		Store(const Null&);
      Status		Store(const Boolean&);
      Status		Store(const Character&);
      Status		Store(const Real&);
      Status		Store(const Large&);
      Status		Store(const String&);
      Status		Store(const Region&);
      Status		Store(const Archive&);

      template <typename T>
      Status		Load(T&);
      Status		Load(Null&);
      Status		Load(Boolean&);
      Status		Load(Character&);
      Status		Load(Real&);
      Status		Load(Large&);
      Status		Load(String&);
      Status		Load(Region&);
      Status		Load(Archive&);

      Status		Fetch(enum Type&);

      //
      // behaviours
      //
      template <typename T, Boolean C>
      struct		Behaviour
      {
	static Status	Serialize(Archive&,
				  const T&);
	static Status	Extract(Archive&,
				T&);
      };

      template <typename T>
      struct		Behaviour<T, true>
      {
	static Status	Serialize(Archive&,
				  const Archivable&);
	static Status	Extract(Archive&,
				Archivable&);
      };

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // object-like
      template <typename T>
      Status		Recycle(const T* = NULL);

      virtual Status	Imprint(Natural32&) const;
      virtual Status	Clone(Archive*&) const;

      Archive&		operator=(const Archive&);
      Boolean		operator==(const Archive&) const;
      Boolean		operator!=(const Archive&) const;

      //
      // attributes
      //
      Mode			mode;
      Control			control;

      Byte*&			contents;
      size_t&			size;
      size_t			offset;

      ::msgpack::sbuffer	buffer;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/package/Archive.hxx>

#endif

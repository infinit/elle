//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/archive/Archive.hh
//
// created       julien quintard   [thu nov  1 21:00:41 2007]
// updated       julien quintard   [mon apr 26 15:44:04 2010]
//

#ifndef ELLE_ARCHIVE_ARCHIVE_HH
#define ELLE_ARCHIVE_ARCHIVE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/miscellaneous/Status.hh>
#include <elle/miscellaneous/Region.hh>

#include <elle/system/System.hh>

#include <elle/idiom/Close.hh>
# include <stdlib.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace miscellaneous;
  using namespace system;

  ///
  /// this namespace contains everything related to serializing and extracting
  /// information.
  ///
  namespace archive
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Archivable;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides methods for serializing basic data types. each
    /// given data is appended to the archive in its original format. the
    /// endianness format is recorded at the head of the archive so that a
    /// machine willing to extract the information knows whether it has to
    /// convert it or not.
    ///
    /// the endianness attribute represents the endianness of the archive,
    /// meaning either the endianness of the current machine if the archive
    /// is being built or the endianness of the machine which built this
    /// archive if information is being extracted.
    ///
    /// the size attribute represents the real size of the archive while the
    /// capacity attribute is the archive's allocated memory size. the offset
    /// attribute is used when extracting information from the archive.
    ///
    /// \todo ajouter la transformation big->little ou little->big pour les
    ///   types qui en ont besoin: integer, natural, bignum principalement.
    ///     http://sources.redhat.com/cgi-bin/cvsweb.cgi/libc/sunrpc/rpc/xdr.h?rev=1.28.2.1&content-type=text/x-cvsweb-markup&cvsroot=glibc
    ///     http://sources.redhat.com/cgi-bin/cvsweb.cgi/libc/sunrpc/xdr_intXX_t.c?rev=1.5.4.2&content-type=text/x-cvsweb-markup&cvsroot=glibc
    ///     http://sources.redhat.com/cgi-bin/cvsweb.cgi/libc/sunrpc/xdr_mem.c?rev=1.15.10.1&content-type=text/x-cvsweb-markup&cvsroot=glibc
    ///     http://sources.redhat.com/cgi-bin/cvsweb.cgi/libc/sunrpc/xdr_float.c?rev=1.6.2.1&content-type=text/x-cvsweb-markup&cvsroot=glibc
    ///
    class Archive:
      public Region
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
      // constants
      //
      static const Character		Magic[3];

      struct Default
      {
	static const Natural64		Capacity;
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
      Status		Prepare(const Region&);

      template <typename T>
      Status		Serialize(const T&);
      template <typename T,
		typename... TT>
      Status		Serialize(const T&,
				  const TT&...);
      template <typename T>
      Status		Extract(T&);
      template <typename T,
		typename... TT>
      Status		Extract(T&,
				TT&...);

      template <typename T>
      Status		Update(const Natural64&,
			       const T&);
      template <typename T,
		typename... TT>
      Status		Update(const Natural64&,
			       const T&,
			       const TT&...);

      template <typename T>
      Status		Store(const T&);
      Status		Store(const Null&);
      Status		Store(const Large&);
      Status		Store(const String&);
      Status		Store(const Region&);
      Status		Store(const Archive&);

      template <typename T>
      Status		Load(T&);
      Status		Load(Null&);
      Status		Load(Large&);
      Status		Load(String&);
      Status		Load(Region&);
      Status		Load(Archive&);

      Status		Fetch(enum Type&);
      Status		Rewind();

      Status		Seal();

      //
      // behaviours
      //
      template <typename T, Boolean C>
      struct		Behaviour
      {
	static Status	Serialize(Archive&, const T&);
	static Status	Extract(Archive&, T&);
      };

      template <typename T>
      struct		Behaviour<T, true>
      {
	static Status	Serialize(Archive&, const Archivable&);
	static Status	Extract(Archive&, Archivable&);
      };

      //
      // interfaces
      //

      // entity
      declare(Entity, Archive);
      Boolean		operator==(const Archive&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Mode		mode;

      System::Order	endianness;

      Natural64		offset;

    private:
      //
      // methods
      //
      Status		Reserve(const Natural64);
      Status		Align(const Natural64);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archivable.hh>

//
// ---------- templates -------------------------------------------------------
//

#include <elle/archive/Archive.hxx>

#endif

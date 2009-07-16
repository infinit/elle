//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/archive/Archive.hh
//
// created       julien quintard   [thu nov  1 21:00:41 2007]
// updated       julien quintard   [wed mar 11 13:09:40 2009]
//

#ifndef ELLE_ARCHIVE_ARCHIVE_HH
#define ELLE_ARCHIVE_ARCHIVE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/misc/Byte.hh>
#include <elle/misc/Region.hh>
#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>

#include <elle/system/System.hh>

#include <elle/archive/Archivable.hh>

#include <stdlib.h>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace misc;
  using namespace system;

  ///
  /// this namespace contains everything related to serializing and extracting
  /// information.
  ///
  namespace archive
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides methods for serializing basic data types. each given
    /// data is appended to the archive in its original format. the endianness
    /// format is recorded at the head of the archive so that a machine willing
    /// to extract the information knows whether it has to convert it or not.
    ///
    /// the endianness attribute represents the endianness of the archive,
    /// meaning either the endianness of the current machine if the archive
    /// is being built or the endianness of the machine which built this archive
    /// if information is being extracted.
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
	  TypeVoid,
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
	static const Natural32		Capacity;
      };

      //
      // constructors & destructors
      //
      Archive()
      {
	Archive::New(*this);
      }

      ~Archive()
      {
	Archive::Delete(*this);
      }

      //
      // methods
      //
      Status		Create();
      Status		Prepare(const Region&);

      template <typename T>
      Status		Serialize(const T&);

      template <typename T>
      Status		Extract(T&);

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

      // object
      static Status	New(Archive&);
      static Status	Delete(Archive&);

      Archive&		operator=(const Archive&);
      Boolean		operator==(const Archive&);
      Boolean		operator!=(const Archive&);

      // dumpable
      Status		Dump(const Natural32 = 0);

      //
      // attributes
      //
      Mode		mode;

      System::Endianness endianness;

      Natural32		offset;

    private:
      //
      // methods
      //
      Status		Reserve(const Natural32);
      Status		Align(const Natural32);

    public:
      //
      // variadic templates
      //

      // serialize
      template <typename T1,
		typename T2>
      Status		Serialize(const T1&,
				  const T2&);
      template <typename T1,
		typename T2,
		typename T3>
      Status		Serialize(const T1&,
				  const T2&,
				  const T3&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Serialize(const T1&,
				  const T2&,
				  const T3&,
				  const T4&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      Status		Serialize(const T1&,
				  const T2&,
				  const T3&,
				  const T4&,
				  const T5&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      Status		Serialize(const T1&,
				  const T2&,
				  const T3&,
				  const T4&,
				  const T5&,
				  const T6&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      Status		Serialize(const T1&,
				  const T2&,
				  const T3&,
				  const T4&,
				  const T5&,
				  const T6&,
				  const T7&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      Status		Serialize(const T1&,
				  const T2&,
				  const T3&,
				  const T4&,
				  const T5&,
				  const T6&,
				  const T7&,
				  const T8&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      Status		Serialize(const T1&,
				  const T2&,
				  const T3&,
				  const T4&,
				  const T5&,
				  const T6&,
				  const T7&,
				  const T8&,
				  const T9&);

      // extract
      template <typename T1,
		typename T2>
      Status		Extract(T1&,
				T2&);
      template <typename T1,
		typename T2,
		typename T3>
      Status		Extract(T1&,
				T2&,
				T3&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Extract(T1&,
				T2&,
				T3&,
				T4&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      Status		Extract(T1&,
				T2&,
				T3&,
				T4&,
				T5&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      Status		Extract(T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      Status		Extract(T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&,
				T7&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      Status		Extract(T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&,
				T7&,
				T8&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      Status		Extract(T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&,
				T7&,
				T8&,
				T9&);
    };

    ///
    /// this type is used internally and should never
    /// be instantiated.
    ///
    struct Void
    {
    };

    extern Void		vacuum;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&	operator<<(std::ostream&,
			   const elle::archive::Archive&);
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/archive/Archive.hxx>

#endif

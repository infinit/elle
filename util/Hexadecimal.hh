//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/util/Hexadecimal.hh
//
// created       julien quintard   [fri apr 16 19:22:17 2010]
// updated       julien quintard   [tue may  4 10:52:59 2010]
//

#ifndef ELLE_UTIL_HEXADECIMAL_HH
#define ELLE_UTIL_HEXADECIMAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/radix/Status.hh>

#include <elle/standalone/Region.hh>

#include <elle/archive/Archive.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace archive;

  namespace util
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for encoding and decoding
    /// hexadecimal.
    ///
    /// note that for non-region objects, the methods first serialize
    /// the object before encoding it. therefore, this class can be
    /// used to serialize objects in human-kind-of-readable formats.
    ///
    class Hexadecimal
    {
    public:
      //
      // constants
      //
      static const String	Alphabet;

      //
      // static methods
      //
      static Status	Validate(const String&);

      static Status	Encode(const Region&,
			       String&);
      static Status	Decode(const String&,
			       Region&);

      template <typename T,
		typename... TT>
      static Status	Decode(const String&,
			       T&,
			       TT&...);

      //
      // forward methods
      //

      ///
      /// this methods are required because the compiler, given an Archive
      /// object will call a template-based method instead of the Plain one.
      ///
      /// we do not want this especially because the template-based methods
      /// build archives and we are already receiving an archive.
      ///

      static Status	Encode(const Archive&		archive,
			       String&			string)
      {
	return (Hexadecimal::Encode((Region&)archive, string));
      }

      Status		Decrypt(const String&		string,
				Archive&		archive)
      {
	return (Hexadecimal::Decode(string, (Region&)archive));
      }

      //
      // variadic templates
      //

      // encrypt
      template <typename T1>
      static Status	Encode(const T1&,
			       String&);
      template <typename T1,
		typename T2>
      static Status	Encode(const T1&,
			       const T2&,
			       String&);
      template <typename T1,
		typename T2,
		typename T3>
      static Status	Encode(const T1&,
			       const T2&,
			       const T3&,
			       String&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      static Status	Encode(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       String&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      static Status	Encode(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       String&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      static Status	Encode(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       String&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      static Status	Encode(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       String&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      static Status	Encode(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       const T8&,
			       String&);
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      static Status	Encode(const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       const T8&,
			       const T9&,
			       String&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/util/Hexadecimal.hxx>

#endif

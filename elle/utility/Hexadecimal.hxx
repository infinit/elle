//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Hexadecimal.hxx
//
// created       julien quintard   [sun apr 18 15:34:47 2010]
// updated       julien quintard   [thu apr 28 13:46:50 2011]
//

#ifndef ELLE_UTILITY_HEXADECIMAL_HXX
#define ELLE_UTILITY_HEXADECIMAL_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- variadic templates ----------------------------------------------
//

    ///
    /// these methods make it easier to encode/decode multiple items at
    /// the same time while keeping a way to catch errors.
    ///
    /// note that the Encode() methods cannot be factorised through
    /// variadic templates since the last argument is not a variadic
    /// argument.
    ///

    //
    // encode
    //

    ///
    /// this method encodes the given object by serializing it.
    ///
    template <typename T1>
    Status		Hexadecimal::Encode(const T1&		o1,
					    String&		string)
    {
      Archive		archive;

      enter();

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o1) == StatusError)
	escape("unable to serialize the object");

      // encode the archive.
      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    const T3&		o3,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    const T3&		o3,
					    const T4&		o4,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    const T3&		o3,
					    const T4&		o4,
					    const T5&		o5,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    const T3&		o3,
					    const T4&		o4,
					    const T5&		o5,
					    const T6&		o6,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5,
			    o6) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    const T3&		o3,
					    const T4&		o4,
					    const T5&		o5,
					    const T6&		o6,
					    const T7&		o7,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5,
			    o6, o7) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    const T3&		o3,
					    const T4&		o4,
					    const T5&		o5,
					    const T6&		o6,
					    const T7&		o7,
					    const T8&		o8,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5,
			    o6, o7, o8) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    template <typename T1,
	      typename T2,
	      typename T3,
	      typename T4,
	      typename T5,
	      typename T6,
	      typename T7,
	      typename T8,
	      typename T9>
    Status		Hexadecimal::Encode(const T1&		o1,
					    const T2&		o2,
					    const T3&		o3,
					    const T4&		o4,
					    const T5&		o5,
					    const T6&		o6,
					    const T7&		o7,
					    const T8&		o8,
					    const T9&		o9,
					    String&		string)
    {
      Archive		archive;

      enter();

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5,
			    o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the object");

      if (Hexadecimal::Encode(archive, string) == StatusError)
	escape("unable to encode object's archive");

      leave();
    }

    //
    // decrypt
    //

    ///
    /// this method decodes the given string before extracting the
    /// object.
    ///
    template <typename T,
	      typename... TT>
    Status		Hexadecimal::Decode(const String&	string,
					    T&			parameter,
					    TT&...		parameters)
    {
      Archive		archive;
      Region		region;

      enter();

      // decode the string.
      if (Hexadecimal::Decode(string, region) == StatusError)
	escape("unable to decode the string");

      // detach the region.
      if (region.Detach() == StatusError)
	escape("unable to detach the region's data");

      // wrap the region into an archive.
      if (archive.Prepare(region) == StatusError)
	escape("unable to prepare the archive");

      // extract the object.
      if (archive.Extract(parameter, parameters...) == StatusError)
	escape("unable to extract the object");

      leave();
    }

  }
}

#endif

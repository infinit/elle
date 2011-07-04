//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Uniquable.hxx
//
// created       julien quintard   [sun may  2 13:22:46 2010]
// updated       julien quintard   [mon jul  4 12:04:27 2011]
//

#ifndef ELLE_IO_UNIQUABLE_HXX
#define ELLE_IO_UNIQUABLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Hexadecimal.hh>
#include <elle/utility/Base64.hh>

namespace elle
{
  using namespace utility;

  namespace io
  {

//
// ---------- hexadecimal -----------------------------------------------------
//

    ///
    /// this specialization is for the hexadecimal format.
    ///
    template <>
    class Uniquable<FormatHexadecimal>:
      public virtual Archivable
    {
    public:
      //
      // methods
      //
      Status		Save(Unique&				unique) const
      {
	enter();

	// encode the object in hexadecimal.
	if (Hexadecimal::Encode(*this, unique) == StatusError)
	  escape("unable to encode the object in hexadecimal");

	leave();
      }

      Status		Restore(const Unique&			unique)
      {
	enter();

	// decode the unique and reconstitute the object.
	if (Hexadecimal::Decode(unique, *this) == StatusError)
	  escape("unable to decode the hexadecimal unique");

	leave();
      }
    };

//
// ---------- base64 ----------------------------------------------------------
//

    ///
    /// this specialization is for the base64 format.
    ///
    template <>
    class Uniquable<FormatBase64>:
      public virtual Archivable
    {
    public:
      //
      // methods
      //
      Status		Save(Unique&				unique) const
      {
	enter();

	// encode the object in base64.
	if (Base64::Encode(*this, unique) == StatusError)
	  escape("unable to encode the object in base64");

	leave();
      }

      Status		Restore(const Unique&			unique)
      {
	enter();

	// decode the unique and reconstitute the object.
	if (Base64::Decode(unique, *this) == StatusError)
	  escape("unable to decode the base64 unique");

	leave();
      }
    };

//
// ---------- custom ----------------------------------------------------------
//

    ///
    /// this specialization is for the custom format.
    ///
    template <>
    class Uniquable<FormatCustom>:
      public virtual Archivable
    {
    public:
      //
      // operators
      //
      virtual Status	Save(Unique&) const
      {
	enter();

	escape("this method should never have been called");
      }

      virtual Status	Restore(const Unique&)
      {
	enter();

	escape("this method should never have been called");
      }
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this function implements the << operator for uniquables.
  ///
  template <const elle::io::Format F>
  ostream&		operator<<(
                          ostream&				stream,
			  const elle::io::Uniquable<F>&		object)
  {
    elle::io::Unique			unique;

    enter();

    // generate the unique identifier.
    if (object.Save(unique) == elle::radix::StatusError)
      yield(stream, "unable to save the object");

    // display the unique in its entirety.
    stream << unique;

    // release.
    release();

    return (stream);
  }

}

#endif

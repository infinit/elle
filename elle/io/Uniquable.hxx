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
// updated       julien quintard   [mon may  3 00:21:27 2010]
//

#ifndef ELLE_IO_UNIQUABLE_HXX
#define ELLE_IO_UNIQUABLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/util/Hexadecimal.hh>
#include <elle/util/Base64.hh>

namespace elle
{
  using namespace util;

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
      // operators
      //
      Void		operator>>(Unique&			unique) const
      {
	// encode the object in hexadecimal.
	Hexadecimal::Encode(*this, unique);
      }

      Void		operator<<(const Unique&		unique)
      {
	// decode the unique and reconstitute the object.
	Hexadecimal::Decode(unique, *this);
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
      // operators
      //
      Void		operator>>(Unique&			unique) const
      {
	// encode the object in base64.
	Base64::Encode(*this, unique);
      }

      Void		operator<<(const Unique&		unique)
      {
	// decode the unique and reconstitute the object.
	Base64::Decode(unique, *this);
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
      virtual Void	operator>>(Unique&			unique) const
      {
	enter();

	alert("this method should never have been called");
      }

      virtual Void	operator<<(const Unique&		unique)
      {
	enter();

	alert("this method should never have been called");
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
  std::ostream&		operator<<(
                          std::ostream&				stream,
			  const
			    elle::io::Uniquable<F>&		object)
  {
    static const elle::core::Natural32	Length = 50;
    elle::io::Unique			unique;

    // generate the unique identifier.
    object >> unique;

    // display the unique depending on its length.
    if (unique.length() < Length)
      {
	// display the unique in its entirety.
	stream << unique;
      }
    else
      {
	// otherwise chop it.
	stream << unique.substr(0, Length / 2)
	       << "..."
	       << unique.substr(unique.length() - (Length / 2));
      }

    return (stream);
  }

}

#endif

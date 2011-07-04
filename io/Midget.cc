//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Midget.cc
//
// created       julien quintard   [thu jun 23 10:30:29 2011]
// updated       julien quintard   [mon jul  4 11:53:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Midget.hh>

namespace elle
{
  namespace io
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Midget::Midget(const Object&				object,
		   const Natural32				length):
      object(object),
      length(length)
    {
    }

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this function implements the << operator for midgets.
  ///
  ostream&		operator<<(
                          ostream&				stream,
			  const elle::io::Midget&		midget)
  {
    elle::io::Unique			unique;

    enter();

    // generate the unique identifier.
    if (midget.object.Save(unique) == elle::radix::StatusError)
      yield(stream, "unable to save the object");

    // display the unique depending on its length.
    if (unique.length() < midget.length)
      {
	// display the unique in its entirety.
	stream << unique;
      }
    else
      {
	// otherwise chop it.
	stream << unique.substr(0, midget.length / 2)
	       << "..."
	       << unique.substr(unique.length() - (midget.length / 2));
      }

    // release.
    release();

    return (stream);
  }

}

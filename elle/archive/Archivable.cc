//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/archive/Archivable.cc
//
// created       julien quintard   [sat mar 20 00:02:21 2010]
// updated       julien quintard   [sun apr 18 16:28:48 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archivable.hh>

///
/// these includes are placed here to prevent pre-processing conflicts.
///
#include <elle/archive/Archive.hh>
#include <elle/miscellaneous/Maid.hh>
#include <elle/miscellaneous/Report.hh>
#include <elle/util/Base64.hh>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace miscellaneous;
  using namespace util;

  namespace archive
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method serializes an object.
    ///
    Status		Archivable::Serialize(Archive&) const
    {
      enter();

      escape("this method should never have been called");
    }

    ///
    /// this method extract an object.
    ///
    Status		Archivable::Extract(Archive&)
    {
      enter();

      escape("this method should never have been called");
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator displays an address in its compact format.
    ///
    std::ostream&	operator<<(std::ostream&		stream,
				   const Archivable&		object)
    {
      String		string;

      enter();

      // encode the object.
      if (Base64::Encode(object, string) == StatusError)
	alert("unable to encode the object", stream);

      // display the base64 string.
      stream << string;

      // release the trackers.
      release();

      return (stream);
    }

  }
}

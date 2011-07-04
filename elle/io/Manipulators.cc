//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Manipulators.cc
//
// created       julien quintard   [mon jul  4 16:45:37 2011]
// updated       julien quintard   [mon jul  4 19:28:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Manipulators.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Maid.hh>

namespace std
{

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// XXX
  ///
  elle::core::String	chop(const elle::core::String&		string,
			     const elle::core::Natural32	limit)
  {
    std::ostringstream	stream;

    enter();

    // display the string depending on its length.
    if (string.length() < limit)
      {
	// display the string in its entirety.
	stream << string;
      }
    else
      {
	// otherwise chop it.
	stream << string.substr(0, limit / 2)
	       << "..."
	       << string.substr(string.length() - (limit / 2));
      }

    // release.
    release();

    return (stream.str());
  }

  ///
  /// XXX
  ///
  template <const elle::io::Format F>
  elle::core::String	chop(const elle::io::Uniquable<F>&	uniquable,
			     const elle::core::Natural32	limit)
  {
    elle::io::Unique	unique;

    enter();

    // generate the unique identifier.
    if (uniquable.Save(unique) == elle::radix::StatusError)
      yield(unique, "unable to save the object");

    // release.
    release();

    // display a chopped uniquable.
    return (std::chop(unique, limit));
  }

}

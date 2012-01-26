//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon jul  4 16:45:37 2011]
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
  /// this method takes a string and a length limit and chop the string
  /// if necessary.
  ///
  /// this method can be used "as a manipulator" like this:
  ///
  ///   std::cout << chop("chiche") << std::endl;
  ///
  elle::core::String    chop(const elle::core::String&          string,
                             const elle::core::Natural32        limit)
  {
    std::ostringstream  stream;

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
  /// this method is equivalent to the previous one but for Uniquable
  /// instances, making it even easier to display such objects.
  ///
  ///   PrivateKey      k;
  ///
  ///   std::cout << chop(k) << std::endl;
  ///
  template <const elle::io::Format F>
  elle::core::String    chop(const elle::io::Uniquable<F>&      uniquable,
                             const elle::core::Natural32        limit)
  {
    elle::io::Unique    unique;

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

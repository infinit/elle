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
  elle::String    chop(const elle::String&          string,
                             const elle::Natural32        limit)
  {
    std::ostringstream  stream;

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
  elle::String    chop(const elle::io::Uniquable<F>&      uniquable,
                             const elle::Natural32        limit)
  {
    elle::io::Unique    unique;

    // generate the unique identifier.
    if (uniquable.Save(unique) == elle::Status::Error)
      yield(unique, "unable to save the object");

    // display a chopped uniquable.
    return (std::chop(unique, limit));
  }

}

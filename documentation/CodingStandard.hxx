//
// ---------- header ----------------------------------------------------------
//
// project       documentation
//
// license       infinit
//
// author        julien quintard   [fri jan 27 15:02:19 2012]
//

#ifndef PACKAGE_MODULE_CODINGSTANDARD_HXX
#define PACKAGE_MODULE_CODINGSTANDARD_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <infinit-dependency-1.hh>
#include <infinit-dependency-2.hh>

#include <idiom/Close.hh>
# include <system-dependecy.h>
# include <library-dependency.h>
#include <idiom/Open.hh>

using namespace package::module

//
// ---------- template methods ------------------------------------------------
//

///
/// description of the method.
///
template <typename T>
Status              CodingStandard::Method3(const Natural32 n,
                                            T&              object)
{
  //
  // do something.
  //

  [...]

  return (StatusOk);
}

#endif

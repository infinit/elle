//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed feb 18 15:20:44 2009]
//

#ifndef ELLE_CORE_NULL_HH
#define ELLE_CORE_NULL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <ostream>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace core
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration is used for specifying that nothing is
    /// to be expected.
    ///
    enum Null
      {
        Nil
      };

//
// ---------- externs ---------------------------------------------------------
//

    extern Null                 none;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ostream&              operator<<(ostream&,
                                   const elle::core::Null&);

}

#endif

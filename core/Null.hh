//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Null.hh
//
// created       julien quintard   [wed feb 18 15:20:44 2009]
// updated       julien quintard   [wed apr  7 22:42:23 2010]
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
    /// XXX
    ///
    enum Null
      {
	Nil
      };

//
// ---------- externs ---------------------------------------------------------
//

    extern Null			none;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const elle::core::Null&);
}

#endif

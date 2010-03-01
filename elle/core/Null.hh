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
// updated       julien quintard   [mon mar  1 12:24:41 2010]
//

#ifndef ELLE_CORE_NULL_HH
#define ELLE_CORE_NULL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <ostream>

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
// ---------- declarations ----------------------------------------------------
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

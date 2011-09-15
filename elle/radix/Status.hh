//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat oct 27 18:10:10 2007]
//

#ifndef ELLE_RADIX_STATUS_HH
#define ELLE_RADIX_STATUS_HH

namespace elle
{
  namespace radix
  {

//
// ---------- enumeration -----------------------------------------------------
//

    ///
    /// this enumeration represents a return status.
    ///
    /// note that almost all the functions and methods of Elle return
    /// this type.
    ///
    enum Status
      {
	StatusUnknown,

	StatusFalse = false,
	StatusTrue = true,

	StatusOk = 42,
	StatusError = 666
      };

  }
}

#endif

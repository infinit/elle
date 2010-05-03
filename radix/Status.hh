//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Status.hh
//
// created       julien quintard   [sat oct 27 18:10:10 2007]
// updated       julien quintard   [mon may  3 20:58:04 2010]
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

//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/miscellaneous/Status.hh
//
// created       julien quintard   [sat oct 27 18:10:10 2007]
// updated       julien quintard   [sat mar 20 13:49:06 2010]
//

#ifndef ELLE_MISCELLANEOUS_STATUS_HH
#define ELLE_MISCELLANEOUS_STATUS_HH

///
/// this namespace encloses the classes provided by the infinit library.
///
namespace elle
{
  namespace miscellaneous
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

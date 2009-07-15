//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/misc/Status.hh
//
// created       julien quintard   [sat oct 27 18:10:10 2007]
// updated       julien quintard   [tue mar 10 01:17:05 2009]
//

#ifndef ELLE_MISC_STATUS_HH
#define ELLE_MISC_STATUS_HH

///
/// this namespace encloses the classes provided by the infinite library.
///
namespace elle
{
  namespace misc
  {

//
// ---------- enumeration -----------------------------------------------------
//

    ///
    /// this enumeration represents a return status.
    ///
    enum Status
      {
	StatusFalse = false,
	StatusTrue = true,
	StatusOk = 42,
	StatusError = 666
      };

  }
}

#endif

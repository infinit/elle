//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri apr 30 17:29:56 2010]
//

#ifndef ELLE_IO_FORMAT_HH
#define ELLE_IO_FORMAT_HH

namespace elle
{
  namespace io
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration defines the multiple formats for displaying
    /// information.
    ///
    enum Format
      {
	FormatUnknown,
	FormatRaw,
	FormatHexadecimal,
	FormatBase64,
	FormatCustom
      };

  }
}

#endif

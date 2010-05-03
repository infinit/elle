//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Format.hh
//
// created       julien quintard   [fri apr 30 17:29:56 2010]
// updated       julien quintard   [sat may  1 11:39:23 2010]
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

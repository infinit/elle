//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/File.hh
//
// created       julien quintard   [thu may 27 16:17:26 2010]
// updated       julien quintard   [thu may 27 17:24:35 2010]
//

#ifndef ELLE_IO_FILE_HH
#define ELLE_IO_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the local file operations.
    ///
    class File
    {
    public:
      //
      // static methods
      //
      static Status	Read(const String&,
			     Region&);
      static Status	Write(const String&,
			      const Region&);
      static Status	Erase(const String&);
    };

  }
}

#endif

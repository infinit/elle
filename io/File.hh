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
// updated       julien quintard   [tue apr 26 15:50:09 2011]
//

#ifndef ELLE_IO_FILE_HH
#define ELLE_IO_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

#include <elle/io/Path.hh>

namespace elle
{
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
      static Status	Read(const Path&,
			     Region&);
      static Status	Write(const Path&,
			      const Region&);
      static Status	Erase(const Path&);
      static Status	Exist(const Path&);
      static Status	Dig(const Path&);
    };

  }
}

#endif

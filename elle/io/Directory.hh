//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Directory.hh
//
// created       julien quintard   [thu may 27 16:17:26 2010]
// updated       julien quintard   [tue apr 26 19:04:37 2011]
//

#ifndef ELLE_IO_DIRECTORY_HH
#define ELLE_IO_DIRECTORY_HH

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
    /// this class abstracts the local directory operations.
    ///
    class Directory
    {
    public:
      //
      // static methods
      //
      static Status	Create(const Path&);
      static Status	Remove(const Path&);
      static Status	Exist(const Path&);
      static Status	Dig(const Path&);
      static Status	Clear(const Path&);
    };

  }
}

#endif

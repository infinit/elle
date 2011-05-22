//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Link.hh
//
// created       julien quintard   [sun may 22 13:07:04 2011]
// updated       julien quintard   [sun may 22 13:17:41 2011]
//

#ifndef ELLE_IO_LINK_HH
#define ELLE_IO_LINK_HH

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
    /// this class abstracts the local link operations.
    ///
    class Link
    {
    public:
      //
      // static methods
      //
      static Status	Create(const Path&,
			       const Path&);
      static Status	Erase(const Path&);
      static Status	Exist(const Path&);
      static Status	Dig(const Path&);
    };

  }
}

#endif

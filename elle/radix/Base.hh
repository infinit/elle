//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Base.hh
//
// created       julien quintard   [thu jun  9 23:50:49 2011]
// updated       julien quintard   [wed jun 22 22:25:24 2011]
//

#ifndef ELLE_RADIX_BASE_HH
#define ELLE_RADIX_BASE_HH

namespace elle
{
  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this template class can be used to build common base for
    /// various classes.
    ///
    template <template <typename...> class T>
    class Base
    {
    public:
      //
      // constructors & destructors
      //
      virtual ~Base()
      {
      }
    };

  }
}

#endif

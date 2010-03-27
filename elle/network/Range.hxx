//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Range.hxx
//
// created       julien quintard   [fri mar 26 17:18:42 2010]
// updated       julien quintard   [sat mar 27 05:59:13 2010]
//

#ifndef ELLE_NETWORK_RANGE_HXX
#define ELLE_NETWORK_RANGE_HXX

namespace elle
{
  namespace network
  {

//
// ---------- dependencies ----------------------------------------------------
//

    ///
    /// this template specializes the Dependency structure with
    /// a dependency D.
    ///
    template <const Character* D>
    struct Dependency<D>
    {
      static const Natural32	First = Range<D>::First;
      static const Natural32	Last = Range<D>::Last;
      static const Natural32	Size = Range<D>::Size;
    };

    ///
    /// this template specializes the Dependency structure with
    /// no dependency.
    ///
    template <>
    struct Dependency<>
    {
      static const Natural32	First = 0;
      static const Natural32	Last = 0;
      static const Natural32	Size = 0;
    };

  }
}

#endif

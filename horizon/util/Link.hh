//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/Link.hh
//
// created       julien quintard   [sat aug  1 21:06:13 2009]
// updated       julien quintard   [sun aug  9 14:19:50 2009]
//

#ifndef PIG_UTIL_LINK_HH
#define PIG_UTIL_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <pig/hole/Hole.hh>

namespace pig
{
  using namespace hole;

  namespace util
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Link
    {
    public:
      //
      // static methods
      //
      static Status	Create(etoile::core::Object&,
			       const elle::crypto::KeyPair&);
      static Status	Store(etoile::core::Object&);

      static Status	Set(etoile::core::Object&,
			    const String&,
			    const elle::crypto::PrivateKey&);
      static Status	Get(const etoile::core::Object&,
			    String&);
    };

  }
}

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/Directory.hh
//
// created       julien quintard   [sat aug  1 21:06:13 2009]
// updated       julien quintard   [mon aug  3 21:22:03 2009]
//

#ifndef PIG_UTIL_DIRECTORY_HH
#define PIG_UTIL_DIRECTORY_HH

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
    class Directory
    {
    public:
      //
      // static methods
      //
      static Status	Create(etoile::core::Directory&,
			       const elle::crypto::KeyPair&);
      static Status	Store(etoile::core::Directory&);

      static Status	Size(const etoile::core::Directory&,
			     Natural32&);
      static Status	Add(etoile::core::Directory&,
			    const String&,
			    const Address&,
			    const PrivateKey&);
      static Status	Remove(etoile::core::Directory&,
			       const String&,
			       const PrivateKey&);
      static Status	Lookup(const etoile::core::Directory&,
			       const String&,
			       Address&);
    };

  }
}

#endif

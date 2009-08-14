//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/File.hh
//
// created       julien quintard   [sat aug  1 21:06:13 2009]
// updated       julien quintard   [sun aug  9 14:23:20 2009]
//

#ifndef PIG_UTIL_FILE_HH
#define PIG_UTIL_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

#include <pig/hole/Hole.hh>

// XXX
#include <pig/util/Path.hh>
// XXX

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
    class File
    {
    public:
      //
      // static methods
      //
      static Status	Create(etoile::core::Object&,
			       const elle::crypto::KeyPair&);
      static Status	Destroy(etoile::core::Object&);
      static Status	Store(etoile::core::Object&);

      static Status	Read(const etoile::core::Object&,
			     const Natural64,
			     Byte*,
			     Natural64&,
			     const PrivateKey&);
      static Status	Write(etoile::core::Object&,
			      const Natural64,
			      const Byte*,
			      const Natural64,
			      const PrivateKey&);
      static Status	Adjust(etoile::core::Object&,
			       const Natural64,
			       const PrivateKey&);

      static Status	Dump(const char*);
    };

  }
}

#endif

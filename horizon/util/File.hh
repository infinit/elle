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
// updated       julien quintard   [tue aug  4 20:46:09 2009]
//

#ifndef PIG_UTIL_FILE_HH
#define PIG_UTIL_FILE_HH

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
    class File
    {
    public:
      //
      // static methods
      //
      static Status	Create(etoile::core::File&,
			       const elle::crypto::KeyPair&);
      static Status	Destroy(etoile::core::File&);
      static Status	Store(etoile::core::File&);

      static Status	Read(const etoile::core::File&,
			     const Natural64,
			     Byte*,
			     Natural64&,
			     const PrivateKey&);
      static Status	Write(etoile::core::File&,
			      const Natural64,
			      const Byte*,
			      const Natural64,
			      const PrivateKey&);
      static Status	Adjust(etoile::core::File&,
			       const Natural64,
			       const PrivateKey&);
    };

  }
}

#endif

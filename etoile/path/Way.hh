//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Way.hh
//
// created       julien quintard   [tue mar 30 23:31:29 2010]
// updated       julien quintard   [fri may 28 17:49:44 2010]
//

#ifndef ETOILE_PATH_WAY_HH
#define ETOILE_PATH_WAY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Length.hh>
#include <etoile/path/Slice.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a string-based path i.e a way.
    ///
    class Way:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Way			Null;

      //
      // constructors & destructors
      //
      Way();
      Way(const Way&);
      Way(const elle::String&);
      Way(const Way&,
	  Slice&);

      //
      // methods
      //
      elle::Status		Capacity(Length&) const;


      // interfaces
      //

      // object
      declare(Way);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      elle::String	path;
    };

  }
}

#endif

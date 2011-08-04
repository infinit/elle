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
// updated       julien quintard   [thu aug  4 12:39:20 2011]
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
      elle::Boolean	operator==(const Way&) const;

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

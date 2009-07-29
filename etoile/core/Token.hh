//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Token.hh
//
// created       julien quintard   [fri jul 24 16:28:56 2009]
// updated       julien quintard   [wed jul 29 17:04:11 2009]
//

#ifndef ETOILE_COMPONENTS_TOKEN_HH
#define ETOILE_COMPONENTS_TOKEN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <ostream>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Token:
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      Token();
      ~Token();

      //
      // methods
      //
      Status		Update(SecretKey&,
			       PublicKey&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //

      Code*		code;
    };

  }
}

#endif

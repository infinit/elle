//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Token.hh
//
// created       julien quintard   [fri jul 24 16:28:56 2009]
// updated       julien quintard   [mon jul 27 08:32:49 2009]
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
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Token:
      public Entity,
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
      Token()
      {
	Token::New(*this);
      }

      ~Token()
      {
	Token::Delete(*this);
      }

      //
      // methods
      //
      Status		Update(SecretKey&,
			       PublicKey&);

      //
      // interfaces
      //

      // object
      static Status	New(Token&);
      static Status	Delete(Token&);

      Token&		operator=(const Token&);
      Boolean		operator==(const Token&);
      Boolean		operator!=(const Token&);

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

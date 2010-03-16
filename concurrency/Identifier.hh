//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Identifier.hh
//
// created       julien quintard   [wed mar  3 13:37:54 2010]
// updated       julien quintard   [mon mar 15 22:56:07 2010]
//

#ifndef ELLE_CONCURRENCY_IDENTIFIER_HH
#define ELLE_CONCURRENCY_IDENTIFIER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/archive/Archive.hh>
#include <elle/io/IO.hh>

#include <elle/idiom/Close.hh>
# include <openssl/rand.h>
# include <openssl/err.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;
  using namespace io;

  namespace concurrency
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// identifiers are used to uniquely identify events, network packets and
    /// so on.
    ///
    class Identifier:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const Identifier	Null;

      //
      // constructors & destructors
      //
      Identifier();

      //
      // methods
      //
      Status		Generate();

      //
      // interfaces
      //

      // entity
      embed(Entity, Identifier);
      Boolean		operator==(const Identifier&) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Natural64		value;
    };

//
// ---------- operators -------------------------------------------------------
//

    Boolean		operator<(const Identifier&,
				  const Identifier&);

  }
}

#endif

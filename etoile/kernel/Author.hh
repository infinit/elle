//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Author.hh
//
// created       julien quintard   [fri aug 21 22:04:45 2009]
// updated       julien quintard   [sun apr  4 14:45:44 2010]
//

#ifndef ETOILE_KERNEL_AUTHOR_HH
#define ETOILE_KERNEL_AUTHOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Proof.hh>
#include <etoile/kernel/Voucher.hh>
#include <etoile/kernel/Role.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the last user to have modified an object.
    ///
    class Author:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      Author();
      ~Author();

      //
      // methods
      //
      Status		Create();
      Status		Create(const Index&);
      Status		Create(const Index&,
			       const Voucher&);

      //
      // interfaces
      //

      // entity
      declare(Entity, Author);
      Boolean		operator==(const Author&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Role		role;
      Proof*		proof;
    };

  }
}

#endif

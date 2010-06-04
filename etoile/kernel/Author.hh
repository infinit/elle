//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Author.hh
//
// created       julien quintard   [fri aug 21 22:04:45 2009]
// updated       julien quintard   [fri may 28 17:42:44 2010]
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
      public elle::Object
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
      elle::Status	Create();
      elle::Status	Create(const Index&);
      elle::Status	Create(const Index&,
			       const Voucher&);

      //
      // interfaces
      //

      // object
      declare(Author);
      elle::Boolean	operator==(const Author&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Role		role;
      Proof*		proof;
    };

  }
}

#endif

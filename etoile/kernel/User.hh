//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/User.hh
//
// created       julien quintard   [thu apr 29 19:34:10 2010]
// updated       julien quintard   [mon may  3 22:56:15 2010]
//

#ifndef ETOILE_KERNEL_USER_HH
#define ETOILE_KERNEL_USER_HH

namespace etoile
{
  namespace kernel
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a user block.
    ///
    class User:
      public PublicKeyBlock
    {
    public:
      //
      // constructors & destructors
      //

      //
      // methods
      //
      elle::Status	Create(const elle::PublicKey&);

      elle::Status	Seal(const user::Agent&,
			     const Access* = NULL);

      elle::Status	Validate(const hole::Address&,
				 const Access* = NULL) const;

      //
      // interfaces
      //

      // object
      declare(User, _());

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Attributes	attributes;
    };

  }
}

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/ImprintBlock.hh
//
// created       julien quintard   [sat may  7 23:26:59 2011]
// updated       julien quintard   [sun may  8 09:08:00 2011]
//

#ifndef NUCLEUS_PROTON_IMPRINTBLOCK_HH
#define NUCLEUS_PROTON_IMPRINTBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>

#include <nucleus/neutron/Subject.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class ImprintBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      ImprintBlock();

      //
      // methods
      //
      elle::Status	Create(const elle::PublicKey&);

      elle::Status	Bind(Address&) const;
      elle::Status	Validate(const Address&) const;

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      struct
      {
	elle::Time		stamp;
	elle::Natural64		salt;
      }				seed;

      struct
      {
	elle::PublicKey		K;
	neutron::Subject	subject;
      }				owner;
    };

  }
}

#endif

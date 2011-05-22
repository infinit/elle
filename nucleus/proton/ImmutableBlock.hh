//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/ImmutableBlock.hh
//
// created       julien quintard   [sat may 21 12:11:53 2011]
// updated       julien quintard   [sat may 21 13:04:46 2011]
//

#ifndef NUCLEUS_PROTON_IMMUTABLEBLOCK_HH
#define NUCLEUS_PROTON_IMMUTABLEBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/proton/Family.hh>

#include <nucleus/neutron/Component.hh>

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
    class ImmutableBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      ImmutableBlock();
      ImmutableBlock(const Family,
		     const neutron::Component);

      //
      // interfaces
      //

      // fileable
      elle::Status	Load(const Network&,
			     const Address&);
      elle::Status	Store(const Network&,
			      const Address&) const;
      elle::Status	Erase(const Network&,
			      const Address&) const;
      elle::Status	Exist(const Network&,
			      const Address&) const;
    };

  }
}

#endif

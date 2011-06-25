//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/MutableBlock.hh
//
// created       julien quintard   [sat may 21 12:27:09 2011]
// updated       julien quintard   [fri jun 24 17:01:58 2011]
//

#ifndef NUCLEUS_PROTON_MUTABLEBLOCK_HH
#define NUCLEUS_PROTON_MUTABLEBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Block.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/proton/Family.hh>
#include <nucleus/proton/Version.hh>
#include <nucleus/proton/Base.hh>

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
    /// the _base attribute is used internally to keep a view of the
    /// block's original state i.e before being modified.
    ///
    class MutableBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      MutableBlock();
      MutableBlock(const Family,
		   const neutron::Component);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // fileable
      elle::Status	Load(const Network&,
			     const Address&,
			     const Version&);
      elle::Status	Store(const Network&,
			      const Address&) const;
      elle::Status	Erase(const Network&,
			      const Address&) const;
      elle::Status	Exist(const Network&,
			      const Address&,
			      const Version&) const;

      //
      // attributes
      //
      Version		version;

      Base		_base;
    };

  }
}

#endif

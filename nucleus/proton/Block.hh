//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Block.hh
//
// created       julien quintard   [mon feb 16 18:47:31 2009]
// updated       julien quintard   [sat may 14 12:24:03 2011]
//

#ifndef NUCLEUS_PROTON_BLOCK_HH
#define NUCLEUS_PROTON_BLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
    /// this class abstracts the notion of storable block of data.
    ///
    /// note that every block is identified by an address.
    ///
    class Block:
      public elle::Object,
      public virtual elle::Fileable<elle::FormatCustom>
    {
    public:
      //
      // constants
      //
      static const elle::String		Extension;

      //
      // constructors & destructors
      //
      Block();
      Block(const Family,
	    const neutron::Component);

      //
      // methods
      //
      elle::Status		Place(const Network&);

      virtual elle::Status	Bind(Address&) const;
      virtual elle::Status	Validate(const Address&) const;

      //
      // interfaces
      //

      // object
      declare(Block);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      // fileable
      elle::Status	Load(const Network&,
			     const Address&);
      elle::Status	Store(const Network&,
			      const Address&) const;
      elle::Status	Erase(const Network&,
			      const Address&) const;
      elle::Status	Exist(const Network&,
			      const Address&) const;

      //
      // attributes
      //
      Network			network;
      Family			family;
      neutron::Component	component;
    };

  }
}

#endif

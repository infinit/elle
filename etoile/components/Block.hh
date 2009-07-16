//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repositories/infinit/core/components/Block.hh
//
// created       julien quintard   [mon feb 16 18:47:31 2009]
// updated       julien quintard   [thu jul 16 11:37:56 2009]
//

#ifndef ETOILE_COMPONENTS_BLOCK_HH
#define ETOILE_COMPONENTS_BLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Address.hh>

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
    class Block:
      public Object,
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
      Block()
      {
	Block::New(*this);
      }

      ~Block()
      {
	Block::Delete(*this);
      }

      //
      // methods
      //
      virtual Status	Seal() = 0;

      Status		Validate(Address&);

      //
      // attributes
      //
      Address		address;

      //
      // interfaces
      //

      // object
      static Status	New(Block&);
      static Status	Delete(Block&);

      Block&		operator=(const Block&);
      Boolean		operator==(const Block&);
      Boolean		operator!=(const Block&);

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const etoile::components::Block&);
}

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       GPL
//
// file          /home/mycure/infinit/infinit/components/Block.hh
//
// created       julien quintard   [mon feb 16 18:47:31 2009]
// updated       julien quintard   [wed mar 11 13:40:29 2009]
//

#ifndef INFINIT_COMPONENTS_BLOCK_HH
#define INFINIT_COMPONENTS_BLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <infinit/components/Address.hh>

namespace infinit
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
				   const infinit::components::Block&);
}

#endif

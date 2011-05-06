//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/ContentHashBlock.hh
//
// created       julien quintard   [tue feb 17 12:33:12 2009]
// updated       julien quintard   [thu may  5 13:40:43 2011]
//

#ifndef NUCLEUS_PROTON_CONTENTHASHBLOCK_HH
#define NUCLEUS_PROTON_CONTENTHASHBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a content hash block is a block whose address is determined by
    /// applying a one-way hash function on its content.
    ///
    /// this way, such blocks are said to be immutable since modifying
    /// such a block implies creating a new block. indeed, since the data
    /// change, the hash of those data as well, so does the address, hence
    /// creating a new block.
    ///
    class ContentHashBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      ContentHashBlock();

      //
      // methods
      //
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
    };

  }
}

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/ContentHashBlock.hh
//
// created       julien quintard   [tue feb 17 12:33:12 2009]
// updated       julien quintard   [thu apr 22 17:14:53 2010]
//

#ifndef ETOILE_KERNEL_CONTENTHASHBLOCK_HH
#define ETOILE_KERNEL_CONTENTHASHBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>
#include <etoile/hole/Block.hh>

namespace etoile
{
  namespace kernel
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
      public hole::Block
    {
    public:
      //
      // constructors & destructors
      //
      ContentHashBlock(const hole::Component&);

      //
      // methods
      //
      Status		Bind();
      Status		Validate(const hole::Address&) const;

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Family.hh>
#include <etoile/hole/Component.hh>

#endif

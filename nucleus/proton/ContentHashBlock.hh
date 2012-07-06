#ifndef NUCLEUS_PROTON_CONTENTHASHBLOCK_HH
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HH

# include <elle/types.hh>
# include <elle/concept/Fileable.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/ImmutableBlock.hh>

# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// a content hash block is a block whose address is determined by
    /// applying a one-way hash function on its content.
    ///
    /// this way, such blocks are said to be immutable since modifying
    /// such a block implies creating a new block. indeed, since the data
    /// changes, the hash of those data as well, so does the address, hence
    /// the creation of a new block.
    ///
    class ContentHashBlock
      : public ImmutableBlock
    // XXX , public elle::concept::Fileable<ContentHashBlock>
    {
    public:
      //
      // constructors & destructors
      //
      ContentHashBlock();
      ContentHashBlock(const neutron::Component);

      //
      // methods
      //
      elle::Status      Bind(Address&) const;
      elle::Status      Validate(const Address&) const;

      //
      // interfaces
      //

      // object
# include <elle/idiom/Open.hh>
      declare(ContentHashBlock);
# include <elle/idiom/Close.hh>

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // XXX ELLE_CONCEPT_FILEABLE_METHODS(ContentHashBlock);
    };

  }
}

# include <nucleus/proton/ContentHashBlock.hxx>

#endif

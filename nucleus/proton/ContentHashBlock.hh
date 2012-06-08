#ifndef  NUCLEUS_PROTON_CONTENTHASHBLOCK_HH
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HH

# include <elle/types.hh>
# include <elle/concept/Fileable.hh>

# include <nucleus/proton/Address.hh>
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
      //, public elle::concept::Fileable<ContentHashBlock>
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

      //ELLE_CONCEPT_FILEABLE_METHODS(ContentHashBlock);
      //// archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);
    };

  }
}

# include <nucleus/proton/ContentHashBlock.hxx>

#endif

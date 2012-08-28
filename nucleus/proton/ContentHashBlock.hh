#ifndef NUCLEUS_PROTON_CONTENTHASHBLOCK_HH
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HH

# include <elle/types.hh>
# include <elle/utility/Time.hh>

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
    class ContentHashBlock:
      public ImmutableBlock
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

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(ContentHashBlock);

      //
      // attributes
      //
    private:
      /// A creation timestamp and a random salt have been introduced
      /// so as to differentiate two ContentHashBlocks with exactly the
      /// same content. For instance, two empty files referencing the
      /// same Access block containing for instance a single record
      /// for the 'everybody' group and no token would end up with the
      /// exact same ContentHashBlock and address.
      ///
      /// Note that referencing counting is not possible in a decentralised
      /// environment, at least without resorting to complicating protocols.
      /// This is why deduplication is not used.
      elle::utility::Time _creation_stamp;
      elle::Natural64 _salt;
    };

  }
}

# include <nucleus/proton/ContentHashBlock.hxx>

#endif

#ifndef NUCLEUS_PROTON_CONTENTHASHBLOCK_HH
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HH

# include <elle/types.hh>
# include <elle/Printable.hh>

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
      //
      // construction
      //
    public:
      ContentHashBlock(); // XXX[to deserialize]
      ContentHashBlock(Network const& network,
                       neutron::Component const component,
                       elle::cryptography::PublicKey const& creator_K);

      //
      // interfaces
      //
    public:
      // block
      virtual
      Address
      bind() const;
      virtual
      void
      validate(Address const& address) const;
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(ContentHashBlock);
      // printable
      virtual
      void
      print(std::ostream& stream) const;
    };

  }
}

# include <nucleus/proton/ContentHashBlock.hxx>

#endif

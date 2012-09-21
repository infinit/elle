#ifndef NUCLEUS_PROTON_PUBLICKEYBLOCK_HH
# define NUCLEUS_PROTON_PUBLICKEYBLOCK_HH

# include <elle/types.hh>
# include <elle/cryptography/PublicKey.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/MutableBlock.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a public key block i.e a mutable block.
    ///
    /// for this construct, a key pair is generated whenever such a block
    /// is author . then, since the address of such a block is computed
    /// by applying a one-way function on the generated public key, and
    /// since this public key never changes, the block's content can be
    /// modified without implying the creation of a new block as for
    /// ContentHashBlocks.
    ///
    class PublicKeyBlock:
      public MutableBlock
    {
    public:
      //
      // constructors & destructors
      //
      PublicKeyBlock(); // XXX[to deserialize]
      PublicKeyBlock(Network const& network,
                     neutron::Component const component,
                     elle::cryptography::PublicKey const& creator_K);

      //
      // methods
      //
      /// XXX
      Address
      bind() const;
      /// XXX
      void
      validate(Address const& address) const;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    public:
      ELLE_ATTRIBUTE_R(elle::cryptography::PublicKey, K);
    };

  }
}

# include <nucleus/proton/PublicKeyBlock.hxx>

#endif

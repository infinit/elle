#ifndef NUCLEUS_PROTON_PUBLICKEYBLOCK_HH
# define NUCLEUS_PROTON_PUBLICKEYBLOCK_HH

# include <elle/types.hh>

# include <cryptography/PublicKey.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/MutableBlock.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

    /// This class represents a public-key-based mutable block.
    ///
    /// For this construct, a key pair is generated whenever such a block
    /// is created. Then, since the address of such a block is computed
    /// by applying a one-way function on the generated public key, and
    /// since this public key never changes, the block's content can be
    /// modified without implying the creation of a new block as for
    /// immutable block such as ContentHashBlocks.
    ///
    /// NOTE: the public key given at construction must be unique i.e
    ///       must have been generated while the private key should be
    ///       used to sign the block.
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
                     cryptography::PublicKey const& creator_K,
                     cryptography::PublicKey const& block_K);

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
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(PublicKeyBlock);
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
    public:
      ELLE_ATTRIBUTE_R(cryptography::PublicKey, block_K);
    };

  }
}

# include <nucleus/proton/PublicKeyBlock.hxx>

#endif

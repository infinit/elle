#ifndef NUCLEUS_PROTON_IMMUTABLEBLOCK_HH
# define NUCLEUS_PROTON_IMMUTABLEBLOCK_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Block.hh>
# include <nucleus/proton/Family.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

    /// This class derives the Block and abstracts the notion of
    /// immutability i.e every modification on the content of an
    /// immutable block inevitably results in the creation of a new
    /// block, with a different address.
    class ImmutableBlock:
      public Block
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      ImmutableBlock(); // XXX[to deserialize]
      ImmutableBlock(Network const& network,
                     Family const family,
                     neutron::Component const component,
                     elle::cryptography::PublicKey const& creator_K);

      /*--------.
      | Methods |
      `--------*/
    private:
      /// XXX
      static
        elle::io::Path
        _path(Address const& address);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // fileable
      ELLE_CONCEPT_FILEABLE_METHODS();
    };

  }
}

#include <nucleus/proton/ImmutableBlock.hxx>

#endif

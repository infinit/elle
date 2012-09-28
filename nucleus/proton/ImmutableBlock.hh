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

    ///
    /// this class derives the Block and abstracts the notion of
    /// immutable block.
    ///
    class ImmutableBlock:
      public Block
    {
    public:
      //
      // constructors & destructors
      //
      ImmutableBlock(); // XXX[to deserialize]
      ImmutableBlock(Network const& network,
                     Family const family,
                     neutron::Component const component,
                     elle::cryptography::PublicKey const& creator_K);

      //
      // methods
      //
    private:
      /// XXX
      static
        elle::io::Path
        _path(Address const& address);

      //
      // interfaces
      //
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // fileable
    public:
      ELLE_CONCEPT_FILEABLE_METHODS();
    };

  }
}

#include <nucleus/proton/ImmutableBlock.hxx>

#endif

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
      ImmutableBlock();
      ImmutableBlock(const Family,
                     const neutron::Component);

      //
      // methods
      //
    private:
      /// XXX
      static
        elle::io::Path
        _path(Network const& network,
              Address const& address);

      //
      // interfaces
      //
    public:
      // object
#include <elle/idiom/Open.hh>
      declare(ImmutableBlock);
#include <elle/idiom/Close.hh>

      // fileable
    protected:
      ELLE_CONCEPT_FILEABLE_METHODS();
    public:
      void
      load(Network const& network,
           Address const& address);
      void
      store(Network const& network,
            Address const& address) const;
      static
      void
      erase(Network const& network,
            Address const& address);
      static
      elle::Boolean
      exists(Network const& network,
             Address const& address);
    };

  }
}

#include <nucleus/proton/ImmutableBlock.hxx>

#endif

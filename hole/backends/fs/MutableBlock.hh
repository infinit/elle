#ifndef HOLE_BACKENDS_FS_MUTABLEBLOCK_HH
# define HOLE_BACKENDS_FS_MUTABLEBLOCK_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Network.hh>
# include <nucleus/proton/Address.hh>

namespace hole
{
  namespace backends
  {
    namespace fs
    {

      class MutableBlock
      {
        //
        // construction
        //
      public:
        MutableBlock(nucleus::proton::Network const& network,
                     nucleus::proton::Address const& address,
                     nucleus::proton::MutableBlock const& block);

        //
        // methods
        //
      public:
        elle::Boolean
        derives() const;

        //
        // attributes
        //
      private:
        nucleus::proton::Network _network;
        nucleus::proton::Address _address;
        nucleus::proton::MutableBlock const& _block;
      };

    }
  }
}

#endif

#ifndef ETOILE_DEPOT_DEPOT_HXX
# define ETOILE_DEPOT_DEPOT_HXX

# include <hole/Hole.hh>

namespace etoile
{
  namespace depot
  {
    template <typename T>
    std::unique_ptr<T>
    Depot::pull(nucleus::proton::Address const& address,
                nucleus::proton::Version const& version)
    {
      std::unique_ptr<nucleus::proton::Block> block;

      block = hole::Hole::Pull(address, version);

      std::unique_ptr<T> type(dynamic_cast<T*>(block.release()));

      if (type.get() == nullptr)
        throw std::runtime_error("the retrieved block is not a "
                                 "corresponding block");

      return type;
    }
  }
}

#endif

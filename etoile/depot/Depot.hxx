#ifndef ETOILE_DEPOT_DEPOT_HXX
# define ETOILE_DEPOT_DEPOT_HXX

namespace etoile
{
  namespace depot
  {

    /// XXX
    template <typename T>
    std::unique_ptr<T>
    Depot::pull(nucleus::proton::Address const& address,
                nucleus::proton::Version const& version =
                nucleus::proton::Version::Latest)
    {
      std::unique_ptr<nucleus::proton::Block> block;

      block = hole::Hole::Pull(address, version);

      std::unique_ptr<T> type(dynamic_cast<T*>(block.get()));

      if (type.get() == nullptr)
        throw std::runtime_error("the retrieved block is not a "
                                 "corresponding block");

      return type;
    }

  }
}

#endif

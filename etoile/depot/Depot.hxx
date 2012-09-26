#ifndef ETOILE_DEPOT_DEPOT_HXX
# define ETOILE_DEPOT_DEPOT_HXX

# include <hole/Hole.hh>

# include <reactor/exception.hh>
# include <elle/concurrency/Scheduler.hh>

namespace etoile
{
  namespace depot
  {
    template <typename T>
    std::unique_ptr<T>
    Depot::pull(nucleus::proton::Address const& address,
                nucleus::proton::Revision const& revision)
    {
      std::unique_ptr<nucleus::proton::Block> block;

      // Check that the address' component corresponds to the
      // type of block requested. For instance, if one requests
      // a block of type Group, the block's address must embed
      // a component 'group'.
      if (address.component != T::component)
        throw reactor::Exception{
            elle::concurrency::scheduler(),
            elle::sprintf("the address' component '%s' does not "
                        "match the type's '%s'",
                        address.component, T::component)
        };

      // Retrieve the block from the storage layer.
      block = hole().pull(address, revision);

      // Cast it to make sure a node did not return a block of another
      // type.
      std::unique_ptr<T> type(dynamic_cast<T*>(block.release()));

      if (type.get() == nullptr)
        throw reactor::Exception{
            elle::concurrency::scheduler(),
            "the retrieved block type does not match the requested one"
        };

      return type;
    }
  }
}

#endif

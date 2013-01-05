#ifndef ETOILE_NEST_POD_HH
# define ETOILE_NEST_POD_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Egg.hh>

namespace etoile
{
  namespace nest
  {
    /// Provide a overlay on top of an egg, especially by holding the
    /// state of the egg's block to know whether it has been detached
    /// from the nest or if it is still attached.
    class Pod
    {
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      /// Define whether the block is attached to the nest.
      enum class State
        {
          attached,
          detached
        };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Pod(std::shared_ptr<nucleus::proton::Egg>& egg);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(State, state);
      ELLE_ATTRIBUTE_RX(std::shared_ptr<nucleus::proton::Egg>, egg);
    };
  }
}

#endif

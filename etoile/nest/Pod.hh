#ifndef ETOILE_NEST_POD_HH
# define ETOILE_NEST_POD_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/utility/Time.hh>

# include <nucleus/proton/fwd.hh>

# include <boost/noncopyable.hpp>

namespace etoile
{
  namespace nest
  {
    /// Provide a overlay on top of an egg, especially by holding the
    /// state of the egg's block to know whether it has been detached
    /// from the nest or if it is still attached.
    class Pod:
      public elle::Printable,
      private boost::noncopyable
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
      /// Construct a pod from the given egg whose ownership is lost
      /// in favor of the pod.
      Pod(std::shared_ptr<nucleus::proton::Egg>& egg);
      /// Likewise but through a rvalue.
      Pod(std::shared_ptr<nucleus::proton::Egg>&& egg);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Pod);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_RW(State, state);
      /// The last time the block has been accessed..
      ELLE_ATTRIBUTE_RW(elle::utility::Time, accessed);
      /// The egg containing the block and its information.
      ELLE_ATTRIBUTE_RX(std::shared_ptr<nucleus::proton::Egg>, egg);
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Pod::State const state);
  }
}

#endif

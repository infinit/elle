#ifndef NUCLEUS_PROTON_ACTION_HH
# define NUCLEUS_PROTON_ACTION_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Address.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {

    /// This class defines an action related to a block being either
    /// pushed or wiped to/from the storage layer.
    class Action:
      public elle::Printable,
      private boost::noncopyable
    {
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      enum Type
        {
          push,
          wipe
        };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Action(Address const& address,
             Block const* block);
      Action(Address const& address);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Returns the block associated with the action. Note that this
      /// method throws if the action is 'wipe'.
      Block const&
      block() const;

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type const, type);
      ELLE_ATTRIBUTE_R(Address const, address);
      ELLE_ATTRIBUTE(Block const*, block);
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Action::Type type);

  }
}

#endif

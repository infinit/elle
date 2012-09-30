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
    /// pushed/wiped to/from the storage layer.
    class Action:
      public elle::Printable,
      private boost::noncopyable
    {
      //
      // enumerations
      //
    public:
      enum Type
        {
          push,
          wipe
        };

      //
      // construction
      //
    public:
      Action(Address const& address,
             Block const* block);
      Action(Address const& address);

      //
      // methods
      //
    public:
      Block const&
      block() const;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE_R(Type const, type);
      ELLE_ATTRIBUTE_R(Address const, address);
      ELLE_ATTRIBUTE(Block const*, block);
    };

    std::ostream&
    operator <<(std::ostream& stream,
                Action::Type type);

  }
}

#endif

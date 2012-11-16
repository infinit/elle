#ifndef NUCLEUS_PROTON_VALUE_HH
# define NUCLEUS_PROTON_VALUE_HH

# include <elle/types.hh>

# include <nucleus/proton/Node.hh>

namespace nucleus
{
  namespace proton
  {

    /// XXX
    class Value:
      public Node
    {
      //
      // methods
      //
    public:
      /// XXX
      virtual
      elle::Boolean
      empty() const = 0;
      /// XXX
      virtual
      elle::String
      mayor() const = 0;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Value);
    };

  }
}

# include <nucleus/proton/Value.hxx>

#endif

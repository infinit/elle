#ifndef NUCLEUS_PROTON_NETWORK_HH
# define NUCLEUS_PROTON_NETWORK_HH

# include <elle/types.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class identifies a network through a unique name.
    ///
    class Network
    {
      //
      // constants
      //
    public:
      static const Network      Null;

      //
      // constructors & destructors
      //
    public:
      Network(elle::String const& name);

      //
      // methods
      //
    public:
      elle::String const&
      name() const;

      //
      // attributes
      //
    private:
      elle::String _name;

      //
      // operators
      //
    public:
      elle::Boolean
      operator==(const Network&) const;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;
    };

  }
}

#include <nucleus/proton/Network.hxx>

#endif

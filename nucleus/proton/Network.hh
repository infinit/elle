#ifndef  NUCLEUS_PROTON_NETWORK_HH
# define NUCLEUS_PROTON_NETWORK_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class identifies a network through a unique name.
    ///
    class Network:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Network      Null;

      //
      // constructors & destructors
      //
      Network();

      //
      // methods
      //
      elle::Status      Create(const elle::String&);

      //
      // attributes
      //
      elle::String      name;

      //
      // interfaces
      //

      // object
      declare(Network);
      elle::Boolean     operator==(const Network&) const;
      elle::Boolean     operator<(const Network&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //// archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);
    };

  }
}

#endif

#ifndef NUCLEUS_PROTON_HANDLE_HH
# define NUCLEUS_PROTON_HANDLE_HH

# include <elle/types.hh>
# include <cryptography/SecretKey.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/proton/Placement.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// XXX
    ///
    /// XXX placement(version inline), address(version offline) et
    ///     block(version inline chargee en RAM)
    ///
    /// XXX contains everything necessary for loading/storing a block
    /// XXX i.e placement or address + secret (if necessary
    class Handle
    {
    public:
      //
      // constants
      //
    public:
      /// XXX
      static Handle const Null;

      //
      // constructors & destructors
      //
    public:
      /// XXX
      Handle();
      /// XXX
      Handle(Handle const& other);

      //
      // methods
      //
    public:
      /// XXX[transform the handle into a placement-based one]
      void
      appoint(Placement const& placement);
      /// XXX
      Placement const&
      placement() const;
      /// XXX
      Address const&
      address() const;
      /// XXX
      void
      address(Address const& address);
      /// XXX
      cryptography::SecretKey const&
      secret() const;
      /// XXX
      void
      secret(cryptography::SecretKey const& secret);

      //
      // operators
      //
    public:
      Handle&
      operator=(Handle const&);
      elle::Boolean
      operator!=(Handle const&) const;
      elle::Boolean
      operator==(Handle const&) const;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Handle);

      //
      // attributes
      //
    private:
      Placement _placement;
      Address _address;
      cryptography::SecretKey _secret;
    };

  }
}

# include <nucleus/proton/Handle.hxx>

#endif

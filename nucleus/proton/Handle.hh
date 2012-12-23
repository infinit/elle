#ifndef NUCLEUS_PROTON_HANDLE_HH
# define NUCLEUS_PROTON_HANDLE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <cryptography/fwd.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/proton/Placement.hh>

namespace nucleus
{
  namespace proton
  {
    /// Represent the location of a block in the nest.
    ///
    /// In term of implementation, whenever the nest is requested for
    /// loading the given handle, an _egg_ is set in the handle so as to
    /// reference the block in its nest.
    ///
    /// A handle can be created two ways, either by providing the address
    /// of the block along with the secret key for decrypting it or by
    /// providing the egg to reference for accessing the block along with
    /// a temporary address (required for computing the handle's footprint).
    class Handle:
      public elle::Printable
    {
      /* XXX
         permanent: address/secret
         volatile: egg/footprint
       */

      //
      // constructors & destructors
      //
    public:
      /// XXX
      Handle();
      /// XXX
      Handle(Address const& address,
             cryptography::SecretKey const& secret);
      /// XXX
      Handle(Placement const& placement,
             Address const& address,
             cryptography::SecretKey const& secret);
      /// XXX
      Handle(Handle const& other);
      /// XXX
      ~Handle();

      //
      // methods
      //
    public:
      /// XXX
      Placement const&
      placement() const;
      /// XXX
      void
      placement(Placement const& placement) // XXX[to remove]
      {
        this->_placement = placement;
      }
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
      operator=(Handle const&); // XXX? to remove
      elle::Boolean
      operator!=(Handle const&) const;
      elle::Boolean
      operator==(Handle const&) const;

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
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Handle);

      //
      // attributes
      //
    private:
      // XXX Egg* egg;
      // XXX[il devrait y avoir un type: memory, nest, storage layer]
      // XXX[et une union? en fait non probablement pas car c'est bien de garder l'addresse]
      Placement _placement;
      Address _address;
      cryptography::SecretKey* _secret;
    };
  }
}

# include <nucleus/proton/Handle.hxx>

#endif

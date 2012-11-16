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
    /// Repreent the location of a block being either in main memory, in
    /// the nest or on the storage layer.
    ///
    /// In term of implementation, the _placement_ is used to reference the
    /// block in the nest, in which case the block could very well be in
    /// main memory or on a disk-based cache. Should the block not be loaded
    /// in the nest, the _address_ indicates the block's address in the storage
    /// layer. Finally, if the block is actually currently loaded in main
    /// memory, the _block_ attribute holds its address.
    ///
    /// Note that a _secret_ is also kept which enable one to decrypt the
    /// referenced block.
    class Handle
    {
      // XXX
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

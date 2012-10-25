#ifndef TRANSFER_TRANSFER_HH
# define TRANSFER_TRANSFER_HH

# include <elle/types.hh>

# include <reactor/network/fwd.hh>

# include <etoile/path/fwd.hh>
# include <etoile/portal/Manifest.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Permissions.hh>

# include <protocol/fwd.hh>

# include <lune/fwd.hh>

# include <Infinit.hh>

namespace satellite
{

  ///
  /// this class implements the transfer satellite.
  ///
  class Transfer
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationFrom,
        OperationTo
      };

    /// Connect and authenticate to Etoile.
    static
    void
    connect();
    /// Attach the given object's identifier to the Infinit
    /// hierarchy, more precisely at the given path.
    static
    etoile::gear::Identifier
    attach(etoile::gear::Identifier const& object,
           elle::String const& path);
    /// Set up the progress updating process and return a chemin
    /// to the progress file so as to speed up opening and updating it.
    static
    etoile::path::Chemin
    from_setup();
    /// Set the progress in the root directory's attributes.
    static
    void
    from_progress(elle::Natural64 increment);
    /// Traverse a directory so as to transfer back everything from
    /// Infinit to the local file system.
    static
    void
    from_traverse(etoile::path::Way const& way,
                  elle::String const& target);
    /// Transfer everything from Infinit to the local file system.
    static
    void
    from(elle::String const& target);
    /// Update the root directory of the Infinit file system
    /// with the total size of the files/directories/links
    /// having been transfered.
    static
    void
    to_update(elle::Natural64 const size);
    /// Creates a file to Infinit.
    static
    elle::Natural64
    to_create(elle::String const& source,
              elle::String const& target);
    /// Creates a symlink to Infinit.
    static
    elle::Natural64
    to_symlink(elle::String const& source,
               elle::String const& target);
    /// Creates a hierarchical tree to Infinit.
    static
    elle::Natural64
    to_dig(elle::String const& path);
    /// Transfer local files/directories to Infinit.
    static
    void
    to(elle::String const& source);

    static reactor::network::TCPSocket* socket;
    static infinit::protocol::Serializer* serializer;
    static infinit::protocol::ChanneledStream* channels;
    static etoile::portal::RPC* rpcs;
    static lune::Descriptor* descriptor;
  };

}

#endif

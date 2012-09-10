#ifndef LUNE_DESCRIPTOR_HH
# define LUNE_DESCRIPTOR_HH

# include <elle/serialize/fwd.hh>
# include <elle/cryptography/Signature.hh>
# include <elle/idiom/Open.hh>

# include <lune/fwd.hh>

# include <hole/Model.hh>
# include <hole/fwd.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>

# include <horizon/Policy.hh>

# include <Version.hh>

namespace lune
{

  ///
  /// this class represents a network descriptor.
  ///
  /// note that the network name is supposed to be unique as it plays the
  /// role of identifier.
  ///
  class Descriptor:
    public elle::radix::Object,
    public elle::concept::MakeFileable<Descriptor>,
    public elle::concept::MakeUniquable<Descriptor>
  {
    //
    // constants
    //
  public:
    static const elle::String           Extension;

    static const elle::Boolean          History;
    static const elle::Natural32        Extent;
    static const elle::Real             Contention;
    static const elle::Real             Balancing;

    //
    // construction
    //
  public:
    Descriptor();
    ~Descriptor();

    //
    // methods
    //
  public:
    elle::Status
    Create(const elle::String id,
           Version const& version,
           const elle::String&,
           const hole::Model&,
           hole::Openness const& openness,
           const nucleus::proton::Address& root,
           nucleus::neutron::Group::Identity const& everybody,
           const elle::Boolean,
           const elle::Natural32,
           const elle::Real&,
           const elle::Real&,
           horizon::Policy const& policy);

    elle::Status        Seal(const Authority&);
    elle::Status        Validate(const Authority&) const;

    /// XXX
    elle::String const&
    id() const;
    /// XXX[is the setter really used? this is probably abnormal]
    void
    id(elle::String const& id);
    /// XXX
    Version const&
    version() const;
    /// XXX
    hole::Openness const&
    openness() const;
    /// XXX
    horizon::Policy const&
    policy() const;
    /// XXX
    nucleus::neutron::Group::Identity const&
    everybody_identity() const;
    /// XXX
    nucleus::neutron::Subject const&
    everybody_subject();

  private:
    /// XXX
    void
    _everybody_subject();
    /// XXX
    static
    elle::io::Path
    _path(elle::String const& network);

    //
    // interfaces
    //
  public:
    // object
    declare(Descriptor);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // serializable
    ELLE_SERIALIZE_FRIEND_FOR(Descriptor);

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();

    void
    load(elle::String const& network);
    void
    store(elle::String const& network) const;
    static
    void
    erase(elle::String const& network);
    static
    elle::Boolean
    exists(elle::String const& network);

    //
    // attributes
    //
  private:
    elle::String  _id;
    Version _version;

    /* XXX
    struct
    {
      struct
      {
        elle::serialize::Format address;
        elle::serialize::Format block;
        elle::serialize::Format content_hash_block;
        elle::serialize::Format contents;
        elle::serialize::Format immutable_block;
        elle::serialize::Format imprint_block;
        elle::serialize::Format mutable_block;
        elle::serialize::Format owner_key_block;
        elle::serialize::Format public_key_block;
        elle::serialize::Format access;
        elle::serialize::Format attributes;
        elle::serialize::Format catalog;
        elle::serialize::Format data;
        elle::serialize::Format 
        elle::serialize::Format 
        elle::serialize::Format 

+identity/descriptor

      } formats;
    } data;
    */

  public: // XXX
    elle::String        name;
    hole::Model         model;
    hole::Openness _openness;
    nucleus::proton::Address root;
    struct
    {
      nucleus::neutron::Group::Identity identity;
      nucleus::neutron::Subject* subject;
    } _everybody;
    elle::Boolean       history;
    elle::Natural32     extent;
    elle::Real          contention;
    elle::Real          balancing;
    horizon::Policy _policy;
    elle::cryptography::Signature     signature;
  };

}

#include <lune/Descriptor.hxx>

#endif

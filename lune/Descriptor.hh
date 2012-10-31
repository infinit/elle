#ifndef LUNE_DESCRIPTOR_HH
# define LUNE_DESCRIPTOR_HH

# include <elle/Authority.hh>
# include <elle/Version.hh>
# include <elle/serialize/fwd.hh>
# include <elle/cryptography/Signature.hh>
# include <elle/idiom/Open.hh>
# include <elle/serialize/Format.hh>
# include <elle/serialize/DynamicFormat.hh>
# include <elle/serialize/construct.hh>

# include <lune/fwd.hh>

# include <hole/Model.hh>
# include <hole/fwd.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>

# include <horizon/Policy.hh>

# include <boost/noncopyable.hpp>

namespace lune
{

  /// This class represents a network descriptor which, as the name
  /// indicates, describes a network's parameters such as the
  /// administrator's public key, the network unique identifier etc.
  /// along with configuration values which could change over time
  /// should the administrator decide so. These configuration values
  /// include a network name, an openness which defines how open
  /// the network is to other users and the policy which defines the
  /// sharing behaviour i.e are files/directories/etc. shared in
  /// read-only with everybody by default, in read-write or kept
  /// private to the object owner. Finally the configuration embeds
  /// the reference version of the Infinit software and the reference
  /// formats of the blocks. These are provided in order to overcome
  /// the issue described below.
  ///
  /// Let us recall that every block when manipulated, lives in main
  /// memory but when transmitted over the network or stored in serialized
  /// (i.e in an architecture-independent format) so as to be rebuilt
  /// by any other computer no matter its platform, operating system etc.
  /// Besides, since the Infinit software evolves, a block, say the
  /// ContentHashBlock for explanatory puroposes, could embed an additional
  /// attribute, for example a timestamp. Therefore, depending on the version
  /// of the Infinit software, a block could be represented (in main memory)
  /// and serialized in two ways, depending on the Infinit software manipulating
  /// it. More precisely, the way a block is serialized and deserialized
  /// (i.e rebuilt in main memory) depends on the serializer, in this case
  /// the ContentHashBlock serializer.
  ///
  /// Let us imagine a scenario involving two nodes. The first X, runs
  /// Infinit in its version 8 while the second, Y, runs Infinit in a newer
  /// version, say 10. X decides to update a block B and therefore publish it
  /// to the network to replicate it. Since X runs Infinit in its version 8,
  /// the block B has been serialized in the latest ContentHashBlock's format
  /// supported by Infinit version 8. Let us say this version of Infinit has
  /// a ContentHashBlock serializer which produces blocks in the format 2.
  /// However, Infinit version 10 embeds an additional attribute so that
  /// such blocks are now serialized in format 3. Therefore, when the block
  /// B, serialized in format 2, is received, the node Y must be able to
  /// deserialize and manipulate it though it has been serialized in an
  /// older format. In this case, Y will load the block B and actually
  /// manipulate a block in the format 2 though the node Y's Infinit version
  /// can support the format 3. This means the additional attribute is
  /// not set.
  ///
  /// Note that one could think that node Y could upgrade the block B to
  /// the newest version. Unfortunately this is not possible because, often,
  /// after having been deserialized, a block is validated. If one upgrades
  /// the block, the validation will fail because of an invalid signature
  /// (generate in format 2) failing to match the current content (recently
  /// upgraded to format 3 with an additional field which did not exist
  /// in format 2). Consequently, the operation consisting in upgrading
  /// a block from a format to the next should be performed at a very
  /// specific moment, depending on the nature of the block. For a
  /// ContentHashBlock (such as B) for instance, since such a block's address
  /// is function of its content, upgrading the block would modify its
  /// content and therefore its address. Thus, any other block referencing
  /// B would have to be updated as well. This is too difficult since we
  /// do not know which block references it. Besides, for some blocks,
  /// especially the ones which embed signatures, these signatures must be
  /// re-generated. Since only certain users can re-generate the signatures
  /// (often the block owner), the upgrading process can only be performed
  /// by the right user, such as the block owner. As a example, for a
  /// ContentHashBlock, the system will perform the upgrading process
  /// (i) the block has been modified so as to minimize the network
  /// communications (ii) if the user is the block owner (iii) and right
  /// before the call to bind() which computes the block address so as
  /// to take all the modifications (the upgrade) into account.
  ///
  /// In order to counter attacks from malicious nodes, the descriptor
  /// embeds the reference formats which are the ones supported by the
  /// version of Infinit also provided in the descriptor. As a rule of
  /// thumb, the network administrator uses its own Infinit version as
  /// a reference. Thus, whenever the administrator updates the Infinit
  /// software, its version along with the supported format are recorded
  /// in the descriptor which is re-sealed. These reference formats are
  /// used whenever a node receives a block in an unkown format e.g a
  /// format higher than the one supported by the user's Infinit version.
  /// In this case, either the version of Infinit is too old to understand
  /// this format or the block is invalid, embedding a format 12345 for
  /// example though this format is supported by no Infinit version. Should
  /// a node receive such a block, it would check the format against the
  /// reference format of the given block type e.g ContentHashBlock. If the
  /// format is higher that the reference, the node would discard the block
  /// as invalid and would continue. Otherwise, a warning would be displayed
  /// to the user so as to update the Infinit software in order to be able
  /// to understand the latest formats.
  ///
  /// Noteworthy is that formats should evolve quite rarely. Thus, the
  /// impropriety of forcing the user to update the Infinit software
  /// should not be too important.
  class Descriptor:
    public elle::concept::MakeFileable<Descriptor>,
    public elle::concept::MakeUniquable<Descriptor>,
    public elle::serialize::DynamicFormat<Descriptor>,
    private boost::noncopyable
  {
    //
    // forward declarations
    //
    struct Meta;
    struct Data;

    //
    // constants
    //
  public:
    static const elle::String           Extension;

    static const elle::Boolean          History;
    static const elle::Natural32        Extent;

    //
    // construction
    //
  public:
    explicit
    Descriptor(elle::String const& network);
    Descriptor(elle::String const& id,
               elle::cryptography::PublicKey const& administrator_K,
               hole::Model const& model,
               nucleus::proton::Address const& root,
               nucleus::neutron::Group::Identity const& everybody,
               elle::String const& name,
               hole::Openness const& openness,
               horizon::Policy const& policy,
               elle::Boolean history,
               elle::Natural32 extent,
               elle::Version const& version,
               elle::Authority const& authority);

    ELLE_SERIALIZE_CONSTRUCT(Descriptor) {}

    //
    // methods
    //
  public:
    /// XXX
    void
    seal(elle::cryptography::PrivateKey const& administrator_k);
    /// XXX
    void
    validate(elle::Authority const& authority) const;
    /// XXX
    Meta const&
    meta() const;
    /// XXX
    Data const&
    data() const;
  private:
    /// XXX
    static
    elle::io::Path
    _path(elle::String const& network);

    //
    // interfaces
    //
  public:
    // dumpable
    elle::Status
    Dump(const elle::Natural32 = 0) const;

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
    struct Meta:
      private boost::noncopyable
    {
      friend class Descriptor;

      //
      // construction
      //
    public:
      Meta(); // XXX[deserializatio instead]
      Meta(elle::String const& id,
           elle::cryptography::PublicKey const& administrator_K,
           hole::Model const& model,
           nucleus::proton::Address const& root,
           nucleus::neutron::Group::Identity const& everybody,
           elle::Boolean history,
           elle::Natural32 extent,
           elle::Authority const& authority);
      ~Meta();

      //
      // methods
      //
    public:
      /// XXX
      void
      validate(elle::Authority const& authority) const;
      /// XXX
      elle::String const&
      id() const;
      /// XXX[is the setter really used? this is probably abnormal]
      void
      id(elle::String const& id);
      /// XXX
      elle::cryptography::PublicKey const&
      administrator_K() const;
      /// XXX
      hole::Model const&
      model() const;
      /// XXX
      nucleus::proton::Address const&
      root() const;
      /// XXX
      nucleus::neutron::Group::Identity const&
      everybody_identity() const;
      /// XXX
      nucleus::neutron::Subject const&
      everybody_subject() const;
      /// XXX
      elle::Boolean
      history() const;
      /// XXX
      elle::Natural32
      extent() const;
    private:
      /// XXX
      void
      _everybody_subject();

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Descriptor);

      //
      // attributes
      //
    private:
      elle::String _id;
      elle::cryptography::PublicKey _administrator_K;
      hole::Model _model;
      nucleus::proton::Address _root;

      struct
      {
        nucleus::neutron::Group::Identity identity;
        nucleus::neutron::Subject* subject;
      } _everybody;

      elle::Boolean _history;
      elle::Natural32 _extent;

      elle::cryptography::Signature _signature;
    } _meta;

    struct Data:
      private boost::noncopyable
    {
      friend class Descriptor;

      //
      // construction
      //
      Data(); // XXX[deserializatio instead]
      Data(elle::String const& name,
           hole::Openness const& openness,
           horizon::Policy const& policy,
           elle::Version const& version);

      //
      // methods
      //
    public:
      /// XXX
      void
      seal(elle::cryptography::PrivateKey const& administrator_k);
      /// XXX
      void
      validate(elle::cryptography::PublicKey const& administrator_K) const;
      /// XXX
      elle::String const&
      name() const;
      /// XXX
      hole::Openness const&
      openness() const;
      /// XXX
      horizon::Policy const&
      policy() const;
      /// XXX
      elle::Version const&
      version() const;
      /// The methods below enables one to access the reference format for
      /// every block type supported by Infinit.
      elle::serialize::Format const&
      format_block() const;
      elle::serialize::Format const&
      format_content_hash_block() const;
      elle::serialize::Format const&
      format_contents() const;
      elle::serialize::Format const&
      format_immutable_block() const;
      elle::serialize::Format const&
      format_imprint_block() const;
      elle::serialize::Format const&
      format_mutable_block() const;
      elle::serialize::Format const&
      format_owner_key_block() const;
      elle::serialize::Format const&
      format_public_key_block() const;
      elle::serialize::Format const&
      format_access() const;
      elle::serialize::Format const&
      format_attributes() const;
      elle::serialize::Format const&
      format_catalog() const;
      elle::serialize::Format const&
      format_data() const;
      elle::serialize::Format const&
      format_ensemble() const;
      elle::serialize::Format const&
      format_group() const;
      elle::serialize::Format const&
      format_object() const;
      elle::serialize::Format const&
      format_reference() const;
      elle::serialize::Format const&
      format_user() const;
      elle::serialize::Format const&
      format_identity() const;
      elle::serialize::Format const&
      format_descriptor() const;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Descriptor);

      //
      // attributes
      //
    private:
      elle::String _name;
      hole::Openness _openness;
      horizon::Policy _policy;
      elle::Version _version;

      struct
      {
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
        elle::serialize::Format ensemble;
        elle::serialize::Format group;
        elle::serialize::Format object;
        elle::serialize::Format reference;
        elle::serialize::Format user;
        elle::serialize::Format identity;
        elle::serialize::Format descriptor;
      } _formats;

      elle::cryptography::Signature _signature;
    } _data;
  };

}

#include <lune/Descriptor.hxx>

#endif

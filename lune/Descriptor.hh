#ifndef LUNE_DESCRIPTOR_HH
# define LUNE_DESCRIPTOR_HH

# include <elle/Version.hh>
# include <elle/serialize/fwd.hh>
# include <elle/cryptography/Signature.hh>
# include <elle/idiom/Open.hh>
# include <elle/serialize/Format.hh>
# include <elle/serialize/DynamicFormat.hh>

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

  /// This class represents a network descriptor.
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
    Descriptor(); // XXX[deserializatio instead]
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
               Version const& version,
               Authority const& authority);

    //
    // methods
    //
  public:
    /// XXX
    void
    seal(elle::cryptography::PrivateKey const& administrator_k);
    /// XXX
    void
    validate(Authority const& authority) const;
    /// XXX
    Meta&
    meta();
    /// XXX
    Data&
    data();
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
           Authority const& authority);
      ~Meta();

      //
      // methods
      //
    public:
      /// XXX
      void
      validate(Authority const& authority) const;
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
      everybody_subject();
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
           Version const& version);

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
      Version const&
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
      Version _version;

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

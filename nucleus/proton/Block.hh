#ifndef NUCLEUS_PROTON_BLOCK_HH
# define NUCLEUS_PROTON_BLOCK_HH

# include <elle/attribute.hh>
# include <elle/concept/Fileable.hh>
# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Digest.hh>
# include <elle/Printable.hh>
# include <elle/utility/Time.hh>
# include <elle/serialize/construct.hh>
# include <elle/serialize/Serializable.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Network.hh>
# include <nucleus/proton/Family.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/neutron/Component.hh>

# include <boost/noncopyable.hpp>

// XXX remove this when design allows Serializable contract to be enforced
# define __NPB_OARCHIVE(...)                                                  \
  elle::serialize::Serializable<__VA_ARGS__>::OutputArchive                   \
  /**/
# define __NPB_IARCHIVE(...)                                                  \
  elle::serialize::Serializable<__VA_ARGS__>::InputArchive                    \
  /**/
# define __NPB_ISTREAM(...)                                                   \
  __NPB_IARCHIVE(__VA_ARGS__)::StreamType                                     \
  /**/
# define __NPB_OSTREAM(...)                                                   \
  __NPB_OARCHIVE(__VA_ARGS__)::StreamType                                     \
  /**/

# define __NPB_DUMP_METHODS(oa, ia, os, is)                             \
  virtual void serialize(oa&) const                                     \
  { elle::unreachable(); }                                              \
  virtual void deserialize(ia&)                                         \
  { elle::unreachable(); }                                              \
  virtual void serialize(os&, oa* = nullptr) const                      \
  { elle::unreachable(); }                                              \
  virtual void deserialize(is&, ia* = nullptr)                          \
  { elle::unreachable(); }                                              \
  /**/


# define __NPB_BREAK_SERIALIZABLE_CONTRACT(...)                               \
  __NPB_DUMP_METHODS(                                                         \
      __NPB_OARCHIVE(__VA_ARGS__),                                            \
      __NPB_IARCHIVE(__VA_ARGS__),                                            \
      __NPB_OSTREAM(__VA_ARGS__),                                             \
      __NPB_ISTREAM(__VA_ARGS__)                                              \
  )                                                                           \
  /**/

namespace nucleus
{
  namespace proton
  {

    /// This class abstracts the notion of storable block of data.
    ///
    /// Note that every block is identified by an address which can be
    /// computed through the bind() method.
    ///
    /// The state attribute indicates whether the block has
    /// been modified; therefore, this indicator is never serialized.
    ///
    /// A block contains some information which indicate the nature
    /// of the block such as _network, _family and _component. In addition,
    /// the block embeds the hash of the block creator so as to be able
    /// to authenticate later, for deleting the block for instance. Finally,
    /// a creation timestamp and a salt are generated at creation so as
    /// to distinguish blocks with the same content. For more information on
    /// the creation timestamp and salt, please refer to the specific physical
    /// blocks, especially to the bind() method which makes use of these to
    /// compute the block address.
    class Block:
      public elle::io::Dumpable,
      public elle::serialize::Serializable<>,
      public elle::concept::Fileable<>,
      public elle::Printable,
      private boost::noncopyable
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      static const elle::String         Extension;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Block(); // XXX[to deserialize]
      ELLE_SERIALIZE_CONSTRUCT(Block)
      {}
      Block(Network const network,
            Family const family,
            neutron::Component const component,
            elle::cryptography::PublicKey const& creator_K);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Computes the address of the block.
      virtual
      Address
      bind() const = 0;
      /// Validates the block's content according to its address.
      virtual
      void
      validate(Address const& address) const = 0;

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // XXX breaks serializable contract. Remove when Block can be an
      // abstract class.
      __NPB_BREAK_SERIALIZABLE_CONTRACT();
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Block);
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      /// Identifies the network in which lies the block.
      ELLE_ATTRIBUTE_R(Network, network);
      /// Identifies the physical nature of the block i.e its
      /// construct so as to ensure its integrity.
      ELLE_ATTRIBUTE_R(Family, family);
      /// Indicates the logical nature of the block i.e the high-level
      /// information.
      ELLE_ATTRIBUTE_R(neutron::Component, component);
      /// The creator attribute is used for authenticating certain
      /// actions such as deleting a block. Given such a request,
      /// a node holding a replica of the block would have to make
      /// sure the requesting user is the block creator. For that
      /// purpose information is kept in the block so as to authenticate
      /// this creator.
      ///
      /// Note however, that just enough information is kept. In this
      /// case, the creator's public key is not kept. Instead, only a
      /// hash is serialized since enough to proceed to an authentication.
      ELLE_ATTRIBUTE(elle::cryptography::Digest, creator);
      /// The block creation timestamp. This timestamp is especially
      /// useful to distinguish two block created by the same creator.
      ELLE_ATTRIBUTE_R(elle::utility::Time, creation_timestamp);
      /// A random salt so as to further differentiate two blocks being
      /// created by the same user at the exact same time.
      ELLE_ATTRIBUTE_R(elle::Natural64, salt);
      /// Indicates the state of the block such as clean or dirty for
      /// example. This attribute is never serialized.
      ELLE_ATTRIBUTE_RW(State, state);
    };

  }
}

# include <nucleus/proton/Block.hxx>

#endif

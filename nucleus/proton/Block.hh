#ifndef NUCLEUS_PROTON_BLOCK_HH
# define NUCLEUS_PROTON_BLOCK_HH

# include <elle/serialize/BufferArchive.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/concept/Fileable.hh>
# include <elle/cryptography/PublicKey.hh>
# include <elle/attribute.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Network.hh>
# include <nucleus/proton/Family.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/neutron/Component.hh>

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

# define __NPB_DUMP_METHODS(oa, ia, os, is)                                   \
virtual void serialize(oa&) const { throw false; }                            \
virtual void deserialize(ia&) { throw false; }                                \
virtual void serialize(os&, oa* = nullptr) const { throw false; }             \
virtual void deserialize(is&, ia* = nullptr) { throw false; }                 \
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

    ///
    /// this class abstracts the notion of storable block of data.
    ///
    /// note that every block is identified by an address.
    ///
    /// the state attribute indicates whether the block has
    /// been modified. therefore, this indicator is never serialized.
    ///
    /// the reader will notice that only the Erase() method is provided
    /// from the Fileable interface. this comes from the fact that the
    /// other methods depend on the nature of the block: mutable or
    /// immutable.
    ///
    class Block:
      public elle::io::Dumpable,
      public elle::serialize::Serializable<>,
      public elle::serialize::Serializable<
        elle::serialize::BufferArchive
      >,
      public elle::concept::Fileable<>
    {
      //
      // constants
      //
    public:
      static const elle::String         Extension;

      //
      // constructors & destructors
      //
    public:
      Block(); // XXX[to deserialize]
      Block(Network const network,
            Family const family,
            neutron::Component const component,
            elle::cryptography::PublicKey const& creator_K);

      //
      // methods
      //
    public:
      /// Computes the address of the block.
      // XXX[should be virtual pure]
      virtual
      Address
      bind() const;
      /// Validates the block's content according to its address.
      // XXX[should be virtual pure]
      virtual
      void
      validate(Address const& address) const;

      //
      // interfaces
      //
    public:

      // XXX breaks serializable contract. Remove when Block can be an
      // abstract class.
      __NPB_BREAK_SERIALIZABLE_CONTRACT();
      __NPB_BREAK_SERIALIZABLE_CONTRACT(elle::serialize::BufferArchive);

      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Block);

      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE_RW(Network, network); // XXX[remove W]
      ELLE_ATTRIBUTE_R(Family, family);
      ELLE_ATTRIBUTE_R(neutron::Component, component);
      ELLE_ATTRIBUTE_R(elle::cryptography::PublicKey, creator_K);
      ELLE_ATTRIBUTE_RW(State, state);
    };

  }
}

# include <nucleus/proton/Block.hxx>

#endif

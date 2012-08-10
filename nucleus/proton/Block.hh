#ifndef NUCLEUS_PROTON_BLOCK_HH
# define NUCLEUS_PROTON_BLOCK_HH

# include <elle/serialize/BufferArchive.hh>
# include <elle/concept/Serializable.hh>
# include <elle/concept/Fileable.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Network.hh>
# include <nucleus/proton/Family.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/neutron/Component.hh>

// XXX remove this when design allows Serializable contract to be enforced
# define __NPB_OARCHIVE(...)                                                  \
  elle::concept::contract::Serializable<__VA_ARGS__>::OutputArchive           \
  /**/
# define __NPB_IARCHIVE(...)                                                  \
  elle::concept::contract::Serializable<__VA_ARGS__>::InputArchive            \
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
    class Block
      : public elle::radix::Object
      , public elle::io::Dumpable
      , public elle::concept::contract::Serializable<>
      , public elle::concept::contract::Serializable<
            elle::serialize::BufferArchive
        >
      , public elle::concept::Fileable<>
    {
    public:
      //
      // constants
      //
      static const elle::String         Extension;

      //
      // constructors & destructors
      //
      Block();
      Block(const Family,
            const neutron::Component);

      //
      // methods
      //
      virtual elle::Status      Bind(Address&) const;
      virtual elle::Status      Validate(const Address&) const;

      //
      // interfaces
      //

      // object
# include <elle/idiom/Open.hh>
      declare(Block);
# include <elle/idiom/Close.hh>

      // XXX breaks serializable contract. Remove when Block can be an
      // abstract class.
      __NPB_BREAK_SERIALIZABLE_CONTRACT();
      __NPB_BREAK_SERIALIZABLE_CONTRACT(elle::serialize::BufferArchive);

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Network                   network; // XXX seems to be always empty??? it should not! must be added to the constructor.
      Family                    family;
      neutron::Component        component;

      State                     state;
    };

  }
}

# include <nucleus/proton/Block.hxx>

#endif

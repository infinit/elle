#ifndef NUCLEUS_PROTON_PORCUPINE_HH
# define NUCLEUS_PROTON_PORCUPINE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>

# include <cryptography/SecretKey.hh>
# include <cryptography/PublicKey.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/proton/Flags.hh>

# include <boost/noncopyable.hpp>

// XXX
# include <nucleus/proton/Radix.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/proton/Tree.hh>

namespace nucleus
{
  namespace proton
  {
    /// XXX
    class Porcupine:
      public elle::io::Dumpable
      // XXX private boost::noncopyable
    {
      //
      // constants
      //
    public:
      /// This constant represents the hard-coded length of the secrets
      /// used to encrypt the blocks composing porcupines.
      /// XXX in bytes & to move in Contents where the encryption actually
      ///     takes place?
      static const elle::Natural32 secret_length;

      // XXX[to remove]
    public:
      enum class Mode
      {
        empty,
          single,
          hierarchy
          };

      //
      // static methods
      //
    public:
      /// This method returns the default secret key to be used whenever
      /// a new handle is created.
      static
      cryptography::SecretKey
      secret();

      //
      // constructors & destructors
      //
    public:
      Porcupine(); // XXX[to deserialize]
      /// XXX[here the agent_K represents the current user which will happen to
      ///     be the creator of every new block]
      Porcupine(/* XXX Network const& network,
                   cryptography::PublicKey const& agent_K,*/
                Nest& nest);

      //
      // methods
      //
    public:
      /// This method returns true if the tree contains no value.
      elle::Boolean
      empty() const;
      /// This method takes a value type in its raw format and takes the
      /// responsibility for attaching it to the porcupine's nest before
      /// adding it.
      template <typename T>
      void
      add(std::unique_ptr<T>&& value);
      /// This method returns true of the given key is associated with a
      /// value.
      template <typename T>
      elle::Boolean
      exist(typename T::K const& k);
      /// XXX
      template <typename T>
      Handle
      lookup(typename T::K const& k);
      /// XXX
      template <typename T>
      Handle
      seek(Capacity const target,
           Capacity& base);
      /// XXX
      template <typename T>
      void
      update(typename T::K const& k);
      /// XXX
      template <typename T>
      void
      remove(typename T::K const& k);
      /// XXX
      template <typename T>
      void
      check(Flags const flags = flags::all);
      /// XXX
      template <typename T>
      void
      walk(elle::Natural32 const margin = 0);
      /// XXX
      template <typename T>
      void
      seal(cryptography::SecretKey const& secret);
      /// XXX
      void
      unseal(cryptography::SecretKey const& secret);
      /// XXX
      template <typename T>
      Statistics
      statistics();
      /// XXX
      Height
      height() const;
      /// XXX
      Capacity
      capacity() const;
      /// XXX
      State
      state() const;
      /// XXX
      Nest&
      nest();
      /// XXX
      void
      nest(Nest& nest);

    private:
      /// This method adds a value to the tree given its key.
      ///
      /// Note that the handle must reference a valid value. Besides, the
      /// added value must have been attached to the porcupine's nest prior
      /// to its insertion.
      template <typename T>
      void
      _add(typename T::K const& k,
           Handle const& v);
      /// XXX
      template <typename T>
      void
      _create();
      template <typename T>
      void
      _create(Handle& handle);
      /// XXX
      template <typename T>
      void
      _optimize();
      /// XXX
      template <typename T>
      Handle
      _search(typename T::K const& k);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin = 0) const;

      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Porcupine);

      //
      // attributes
      //
      // XXX private:
    public:
      Mode _mode;
      Height _height;
      Capacity _capacity;
      ELLE_ATTRIBUTE_X(Handle, root); // XXX[remove X}]

      ELLE_ATTRIBUTE_R(Network, network);
      ELLE_ATTRIBUTE_R(cryptography::PublicKey, agent_K);
      Nest* _nest;
      State _state;
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Porcupine::Mode const mode);
  }
}

# include <nucleus/proton/Porcupine.hxx>

#endif

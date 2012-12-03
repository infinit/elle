#ifndef NUCLEUS_PROTON_PORCUPINE_HH
# define NUCLEUS_PROTON_PORCUPINE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>

# include <cryptography/SecretKey.hh>
# include <cryptography/PublicKey.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/proton/Flags.hh>
# include <nucleus/proton/Nature.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/Tree.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {
    /// XXX
    template <typename T>
    class Porcupine:
      public elle::Printable,
      private boost::noncopyable
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      /// This constant represents the hard-coded length of the secrets
      /// used to encrypt the blocks composing porcupines.
      /// XXX in bytes & to move in Contents where the encryption actually
      ///     takes place?
      static const elle::Natural32 secret_length;

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// This method returns the default secret key to be used whenever
      /// a new handle is created.
      static
      cryptography::SecretKey
      secret();

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// XXX[here the agent_K represents the current user which will happen to
      ///     be the creator of every new block]
      /// XXX[on pourrait passer la secretkey pour dechiffrer plutot que
      ///     d'appeler unseal()]
      Porcupine(Radix const& radix,
                cryptography::SecretKey const& secret,
                /* XXX Network const& network,
                   cryptography::PublicKey const& agent_K,*/
                Nest& nest);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Return true if the porcupine contains no element.
      elle::Boolean
      empty() const;
      /// Return true of the given key is associated with a value.
      elle::Boolean
      exist(typename T::K const& k);
      /// Return the value associated with the given key _k_.
      Door<T>
      lookup(typename T::K const& k);
      /// Take the target index capacity and return the value responsible
      /// for it along with its base capacity index i.e the capacity index
      /// of the first element in the returned value.
      ///
      /// This method enables one to look for elements based on an index
      /// rather than a key, mechanism which is useful in many cases like
      /// for directories whose entries are often retrieved according to
      /// a range [index, size].
      std::pair<Door<T>, Capacity>
      seek(Capacity const target);
      /// Insert the given tuple key/element in the value responsible for
      /// the given key _k_.
      ///
      /// Note that, should this method be used, the type T must provide
      /// a method complying with the following prototype:
      ///
      ///   void
      ///   insert(typename T::K const& k,
      ///          E* e);
      template <typename E>
      void
      insert(typename T::K const& k,
             E* e);
      /// Erase the element corresponding to the given key _k_ from the
      /// value holding it.
      ///
      /// Note that, should this method be used, the type T must provide
      /// a method complying with the following prototype:
      ///
      ///   void
      ///   erase(typename T::K const& k);
      void
      erase(typename T::K const& k);
      /// Make sure the porcupine is consistent following the modification of
      /// the value responsible for the given key _k_.
      void
      update(typename T::K const& k);
      /// Return the number of elements being stored in the porcupine.
      elle::Size
      size() const;
      /// Check that the porcupine is valid according to some points given by
      /// _flags_ such that the internal capacity corresponds to the actual
      /// number of elements being stored, that the block addresses are correct,
      /// that the major keys are indeed the highest in their value and so on.
      ///
      /// This method is obviously provided for debugging purpose and should not
      /// be used in production considering the amount of computing such a check
      /// takes: all the blocks are retrieved from the storage layer and loaded
      /// in memory for checking.
      void
      check(Flags const flags = flags::all);
      /// Return statistics on the porcupine such as the number of blocks
      /// composing it, the average footprint, minimum/maximum capacity etc.
      Statistics
      statistics();
      /// Display a detailed state of the porcupine.
      void
      dump(elle::Natural32 const margin = 0);
      /// Return the radix of the porcupine, once encrypted and sealed.
      ///
      /// The radix could then be serialized or used for instantiate a
      /// porcupine. However, one should be aware of the fact that, depending
      /// on the nature of the content, a radix alone is useless. Given a block
      /// or tree nature, the constituing blocks are encrypted and stored in
      /// the nest. Therefore, while the radix represent the meta descriptor
      /// of the content, the blocks are actually located in the nest.
      ///
      /// Note that once sealed, no modifying operating should be carried
      /// out on the porcupine.
      Radix
      seal(cryptography::SecretKey const& secret);

    private:
      /// Transform an empty porcupine into a value-based porcupine so
      /// as to be able to return the caller a value on which to operate,
      /// for inserting or exploring for example.
      void
      _create();
      /// Represent the key functionality of the porcupine abstraction. This
      /// method does one fundamental thing: it transforms content from one
      /// nature to another e.g from a direct value to a block-based value
      /// or to a block to a tree.
      ///
      /// Such a decision is made depending on the limits associated with
      /// every nature. For example, should the value block reach a footprint
      /// of 1024, it would be transformed into a tree. Likewise, should the
      /// value block reach a low limit of 256 bytes, it would be transformed
      /// into a direct value.
      ///
      /// This mechanism is crucial to transparently adapt the nature of the
      /// content so as to be optimised according to some limits.
      void
      _optimize();

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Nature, nature);
      union
      {
        /// XXX
        T* _value;
        /// XXX
        Handle* _block;
        /// XXX
        Tree<T>* _tree;
      };

      ELLE_ATTRIBUTE(Network, network); // XXX
      ELLE_ATTRIBUTE(cryptography::PublicKey, agent_K); // XXX

      ELLE_ATTRIBUTE(Nest&, nest);
      ELLE_ATTRIBUTE(State, state);
    };
  }
}

# include <nucleus/proton/Porcupine.hxx>

#endif

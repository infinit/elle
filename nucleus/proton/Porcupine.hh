#ifndef NUCLEUS_PROTON_PORCUPINE_HH
# define NUCLEUS_PROTON_PORCUPINE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <cryptography/SecretKey.hh>
# include <cryptography/PublicKey.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/State.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class makes it easy to build a tree-based data structure of
    /// blocks which does not require all the blocks to remain in main
    /// memory.
    ///
    /// every data item is referenced in an inlet. inlets are grouped together
    /// in a quill which represents a leaf of the given tree. then, every
    /// nodule being a seam or quill is referenced by a seam which encapsulates
    /// several of these references i.e inlets.
    ///
    /// therefore, the tree looks a bit like below with _k_ a key, _@_
    /// the address of a block and _d_ the actual data item:
    ///
    ///                             root seam nodule
    ///                          [ k@ | k@ | ... | k@ ]
    ///                             '               `
    ///                            '                 `
    ///              internal seam nodule         internal seam nodule
    ///             [ k@ | k@ | ... | k@ ]       [ k@ | k@ | ... | k@ ]
    ///                     '
    ///                      '
    ///                leaf quill nodule
    ///              [ k@ | k@ | ... | k@ ]
    ///                                 '
    ///                                  '
    ///                               data item
    ///                                 [ d ]
    ///
    /// note that every nodule (seam and quill) is stored in a block, very
    /// much like data items (Data, Catalog, Reference). since the size of
    /// such blocks is limited by the network descriptor's extent attribute,
    /// nodules and data items are split when full. likewise, should a nodule
    /// become too small, a balancing or merging procedure is triggered in
    /// order to `simplify' the tree.
    ///
    /// since such blocks can be quite large, the seam and quill entries,
    /// known as inlets, are organised in a tree-based data structure such
    /// as a map so as to speed up the look up process.
    ///
    /// note that a mayor key refers to the key with the highest value. this
    /// is the key which is propagated through the tree in order to route
    /// the request from the top i.e root nodule. likewise, a key is said to
    /// be the maiden key if it is the only one remaining in a nodule.
    ///
    /// noteworthy is that every nodule and data item must provide methods
    /// for encrypting/decrypting the block content. note that a class
    /// Contents is provided by nucleus so as to ease this process. every
    /// class deriving Contents can very simply be encrypted before being
    /// serialized so as to be stored or sent on the network. the type
    /// T must therefore derive Contents. one will notice that both nodule
    /// classes Seam and Quill along with data item classes Data, Catalog
    /// and Reference derive Contents.
    ///
    /// note that every block is referenced by a handle which either
    /// references the block being loaded in main memory, through a placement,
    /// or not loaded yet, through an address.
    ///
    /// XXX revoir texte
    ///
    /// XXX T doit implamtner des methodes genre Mayor() en plus de deriver Contents
    ///
    /// XXX three views for porcupine: unknown(empty), value, tree
    ///
    /// XXX the tree can be used in two ways: to add/remove values or to lookup a
    ///     value, update it manually and refresh the tree.
    ///
    /// XXX k=key, v=value(catalog handle for instance)
    /// XXX le extent s'applique au Node qui est contenu dans Contents. autrement dit,
    /// la taille finale c'est Contents + Node, Node etait inferieur extent; l'ensemble
    /// peut donc depasser extent. Ce qui est important c'est que ca oscille autour
    /// d'extent.
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
      /// XXX in bytes
      static const elle::Natural32 secret_length;

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
      // types
      //
    public:
      /// XXX
      typedef elle::Natural16 Flags;

      //
      // constants
      //
    public:
      static const Flags FlagNone;
      static const Flags FlagRecursive;
      static const Flags FlagKey;
      static const Flags FlagAddress;
      static const Flags FlagCapacity;
      static const Flags FlagFootprint;
      static const Flags FlagState;
      static const Flags FlagAll;

      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      enum class Mode
        {
          empty,
          value,
          hierarchy
        };

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

      /// This method adds a value to the tree given its key.
      ///
      /// Note that the handle must reference a valid value. Besides, the
      /// added value must have been attached to the porcupine's nest prior
      /// to its insertion.
      template <typename T>
      void
      add(typename T::K const& k,
          Handle const& v);
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
      exists(typename T::K const& k);
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
      check(Flags const flags = FlagAll);
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
      Handle _root;

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

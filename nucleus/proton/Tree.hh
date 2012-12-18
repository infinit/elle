#ifndef NUCLEUS_PROTON_TREE_HH
# define NUCLEUS_PROTON_TREE_HH

# include <elle/types.hh>

# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/Capacity.hh>
# include <nucleus/proton/Height.hh>
# include <nucleus/proton/Root.hh>
# include <nucleus/proton/Flags.hh>
# include <nucleus/proton/State.hh>
# include <nucleus/proton/Statistics.hh>

# include <cryptography/fwd.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {
    /// Represent a tree of blocks so as to provide an efficient, scalable
    /// and extremely flexible data structure for content storage and access.
    ///
    /// This class (along with others such as Quill, Seam, Nodule, Node, Value,
    /// Inlet, Handle etc.) makes it easy to build a tree-based data structure
    /// of blocks which does not require all the blocks to be in main memory for
    /// being manipulated. This way, one can very easily explore a branch of the
    /// tree, or even add some elements without having to retrieve all the
    /// constituent blocks.
    ///
    /// Every data item or element is referenced in an inlet. Inlets are grouped
    /// together in a quill which represents a leaf of the given tree. Then,
    /// every nodule (being a seam or quill) is referenced by a seam (an
    /// internal nodule) which encapsulates several of these references i.e
    /// inlets.
    ///
    /// A tree therefore looks like the one below with _k_ a key, _@_
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
    ///                               value node
    ///                          [ d | d | d | d | d ]
    ///
    /// Note that every nodule (seam and quill) is stored in a block, very
    /// much like values (Data, Catalog, Reference for instance). Since the
    /// size of such blocks must be limited so as to benefict from the
    /// efficiency of such a data structure, Limits are attached to every
    /// node. This way, whenever the size (i.e footprint) of a block reaches
    /// a threshold, the block is split so as to extend the tree data structure.
    /// Likewise, should a node become too small, a balancing or merging process
    /// is triggered in order to 'simplify' the tree.
    ///
    /// Note that a mayor key refers to the key with the highest value. This
    /// is the key which is propagated through the tree in order to route
    /// the request from the top i.e root nodule. Similarly, a key is said to
    /// be the maiden key if it is the only one remaining in a nodule.
    ///
    /// Noteworthy is that nodes (i.e nodules and values), although providing
    /// a serialization method, are never converted into blocks. Indeed, the
    /// content of such nodes must be protected through encryption. In order to
    /// ease the process of encrypting/decrypting such nodes, a Contents class
    /// is provided which takes a node, serializes it and encrypts it.
    ///
    /// Every block constituing the tree is referenced by a handle which either
    /// references the block being loaded in main memory, through a Placement,
    /// or not loaded yet, through an Address.
    ///
    /// The hierarchy of classes is actually quite simple with basically two
    /// branches deriving Node: the nodes which compose the tree data structure
    /// and the values i.e classes representing a set of elements being stored:
    ///
    ///                        Node
    ///                      '     '
    ///                     '       '
    ///                 Nodule     Value
    ///                 '   '        '
    ///                '     '        '
    ///              Seam   Quill      T
    ///
    /// It is important to realize that the tree can be used in two ways. In the
    /// first, one manually add and remove complete value instances from the
    /// tree. For this technique to work, one must be aware that inserting a
    /// complete set of elements (the ones located in the value being added)
    /// not violate the tree consistency. In other words, assuming the value V
    /// being added is placed after the value S, all the elements contained in
    /// V must have a higher key than the mayor key in S and must obviously be
    /// lower than the smallest key in the following value. Therefore,
    /// manipulating the tree manually can become cumbersome, often forcing the
    /// programmer to manually move elements between value instances. The second
    /// way of manipulating the tree consists in looking up the value
    /// responsible for a key, say the one corresponding to the element we
    /// intend to add to the tree. Then, the value can be modified by directly
    /// inserting the element, remove others etc. depending on the interface
    /// provided by the value class T. Finally, the method update(key) can
    /// be called so as to specify the tree that the value responsible for the
    /// given key has been modified. The tree then adapts itself, adjusting the
    /// routing keys and possibly splitting, balancing and merging so as to
    /// optimize the data structure.
    ///
    /// A tree is parameterised by a type T. This type must comply with the
    /// following requirements:
    ///
    ///   o A type T::K must be provided representing the key. This type must
    ///     obviously be comparable through the common operators: <, >=, ==, !=
    ///     etc.
    ///   o The type T must derive Value and therefore comply with the
    ///     requirements of the type Value.
    ///
    /// It is important to understand the roles of limits in the process of
    /// splitting, merging and balancing inlets and elements between nodes.
    /// The extent, balancing and contention parameters composed a Limits
    /// operate on the footprint of a node, not on the footprint of the final
    /// contents block which embeds the node in its encrypted form.
    /// This limits ensures that no node's footprint will exceed the extent,
    /// say of 1024 bytes for example. However, the final contents block
    /// (which encapsulates the node), when serialized, could very well exceed
    /// this limit. This is not considered as problematic because the purpose
    /// of extent limits is more to homogenize the size of the blocks rather
    /// than making sure all the blocks have the exact same size, which is
    /// impossible to guarantee anyway.
    template <typename T>
    class Tree:
      public elle::Printable,
      private boost::noncopyable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Construct a tree ready to receive key/value tuple.
      Tree(Nest&);
      /// Construct a tree based on the given _root_ information.
      Tree(Root const& root,
           cryptography::SecretKey const& secret,
           Nest& nest);
      /// Destruct the tree.
      ///
      /// Note that the representation is destructed, not its constutent
      /// blocks. For these blocks to be destroyed, one must manually call
      /// the destroy() method.
      ~Tree();

      /*--------.
      | Methdos |
      `--------*/
    public:
      /// Return true of the given key is associated with a value element.
      elle::Boolean
      exist(typename T::K const& k);
      /// Take a tuple key/value and add it to the tree.
      void
      add(typename T::K const& k,
          Handle const& v);
      /// Remove the value associated with the given key _k_.
      void
      remove(typename T::K const& k);
      /// Return the handle corresponding to the element associated with the
      /// given key.
      ///
      /// Note that a copy is returned because the block containing this handle
      /// not not be loaded in main memory once the handle is returned.
      Handle
      lookup(typename T::K const& k);
      /// Take the target index capacity and return the value responsible
      /// for it along with its base capacity index i.e the capacity index
      /// of the first element in the returned value.
      ///
      /// This method enables one to look for elements based on an index
      /// rather than a key, mechanism which is useful in many cases like
      /// for directories whose entries are often retrieved according to
      /// a range [index, size].
      std::pair<Handle, Capacity>
      seek(Capacity const target);
      /// Make sure the tree is consistent following the modification of
      /// the value block responsible for the given key _k_.
      void
      update(typename T::K const& k);
      /// Return the handle of the very first node.
      Handle
      head();
      /// Return the handle of the last node composing the tree.
      Handle
      tail();
      /// Check that the tree is valid according to some points given by
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
      /// Return statistics on the tree such as the number of blocks
      /// composing it, the average footprint, minimum/maximum capacity etc.
      Statistics
      statistics();
      /// Display a detailed state of the porcupine.
      void
      dump(elle::Natural32 const margin = 0);
      /// Return the root of the tree, once encrypted and sealed.
      ///
      /// The root could then be serialized or used for instantiate a new
      /// treer. However, one should be aware of the fact that a root alone
      /// is useless since the tree's constituing blocks are located in
      /// the nest.
      ///
      /// Note that once sealed, no modifying operating should be carried
      /// out on the tree.
      Root
      seal(cryptography::SecretKey const& secret);
      /// Detach from the nest all the blocks consituing the tree.
      ///
      /// This method is useful whenever one want to prepare the removal
      /// of all the content blocks.
      ///
      /// Note that this call is final, as for seal(), meaning that the
      /// porcupine cannot be used once destroyed.
      ///
      /// This method is an alternative to erasing all the elements from
      /// the tree, in which case all the blocks would also be destroyed.
      /// This method is however straightforward and therefore far more
      /// efficient.
      void
      destroy();
      /// Return the root block handle.
      Handle const&
      root() const;
      /// Return the root block handle.
      Handle&
      root();

    private:
      /// Return the handle of the quill block responsible for the given key
      /// _k_.
      ///
      /// This method is widely used for inserting, locating and deleting
      /// elements from the quill nodes.
      Handle
      _search(typename T::K const& k);
      /// XXX
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
    public:
      ELLE_ATTRIBUTE(Handle*, root);
      ELLE_ATTRIBUTE_R(Height, height);
      ELLE_ATTRIBUTE_R(Capacity, capacity);

      ELLE_ATTRIBUTE(Nest&, nest);
      ELLE_ATTRIBUTE_R(State, state);
    };
  }
}

# include <nucleus/proton/Tree.hxx>

#endif

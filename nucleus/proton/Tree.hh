#ifndef NUCLEUS_PROTON_TREE_HH
# define NUCLEUS_PROTON_TREE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <nucleus/proton/Height.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/Flags.hh>

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
    class Tree:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Tree();

      /*--------.
      | Methods |
      `--------*/
    public:
      // XXX[put all the tree-related methods in here]

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Tree);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Radix);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_RW(Height, height);
      ELLE_ATTRIBUTE_RX(Handle, root);
    };
  }
}

// XXX # include <nucleus/proton/Tree.hxx>

#endif

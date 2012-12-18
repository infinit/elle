#ifndef NUCLEUS_PROTON_VALUE_HH
# define NUCLEUS_PROTON_VALUE_HH

# include <elle/types.hh>

# include <nucleus/proton/Node.hh>

namespace nucleus
{
  namespace proton
  {
    /// Abstract the notion of value which is a leaf of the tree.
    ///
    /// Every class C deriving Value must provide the following methods.
    /// Besides, every value must provides a type K which represents the
    /// key i.e the type of the indexing element in the Porcupine hierarchy.
    ///
    ///   // Return true if the value is empty.
    ///   elle::Boolean
    ///   empty() const;
    ///
    ///   // Return the element within the value with the highest key.
    ///   C::K
    ///   mayor() const;
    ///
    ///   // Split the value and return a additional value repesenting the
    ///   // right neighbour of the current value.
    ///   //
    ///   // Note that the ownership over the returned value is transferred to
    ///   // the caller.
    ///   C*
    ///   split();
    ///
    ///   // Merge the _other_ value with the current one. Note that the given
    ///   // value can be either the left or right neighbour. In any cases, the
    ///   // two values will be merged in the current one.
    ///   void
    ///   merge(C& other);
    ///
    /// XXX les protos de split et merge ont changes!?!
    /// XXX capacity() prototype
    class Value:
      public Node
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Value();
      Value(Value const&) = default;

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Value);
    };
  }
}

# include <nucleus/proton/Value.hxx>

#endif

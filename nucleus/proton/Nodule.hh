#ifndef NUCLEUS_PROTON_NODULE_HH
# define NUCLEUS_PROTON_NODULE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <cryptography/fwd.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Node.hh>
# include <nucleus/proton/Flags.hh>
# include <nucleus/proton/Porcupine.hh>

namespace nucleus
{
  namespace proton
  {
    /// Abstract the notion of tree node in a Nodule.
    ///
    /// Every nodule must comply with the interface defined below.
    template <typename T>
    class Nodule:
      public Node
    {
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      /// Define the type of nodule. A seam is an internal tree node
      /// while a quill is a leaf node.
      enum class Type
        {
          seam,
          quill
        };

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Transfer a number of inlets from the _left_ nodule to the _right_.
      /// The process stops when the _left_ nodule is left with _size_ bytes of
      /// inlets i.e footprint.
      ///
      /// Note that the _right_ nodule is supposed to contain higher names
      /// so that the inlets from the _left_ nodule with the highest names
      /// are moved to _right_ in order to maintain consistency.
      template <typename X>
      static
      void
      transfer_right(X& left,
                     X& right,
                     Extent const size);
      /// Does the same as transfer_right() by moving inlets from the _right_
      /// nodule to the _left_.
      template <typename X>
      static
      void
      transfer_left(X& left,
                    X& right,
                    Extent const size);
      /// Optimize a given nodule _nodule_ given the fact that the the child
      /// nodule responsible for the key _k_ has been modified.
      ///
      /// In other words, _nodule_ is not the nodule which has been modified:
      /// its child (i.e the one responsible for the given key) has been.
      template <typename X>
      static
      void
      optimize(X& nodule,
               typename T::K const& k);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Construct a nodule given its final type i.e seam or quill.
      Nodule(Type const type);

      /*----------------.
      | Virtual Methods |
      `----------------*/
    public:
      /// Recursively add the tuple key/value to the nodule.
      ///
      /// Thus, only the final nodule---i.e quill---should be responsible
      /// for inserting the tuple. However all the nodules on the way should
      /// make sure to adapt to the fact that the impacted child nodule may
      /// have changed to a point where a limit is reached.
      ///
      /// Note that 'recursively' means that the nodule must 'route' the call
      /// to the child nodule responsible for the given key.
      virtual
      void
      add(typename T::K const& k,
          Handle const& v) = 0;
      /// Similar to the add() method but for removing a key/value tuple.
      virtual
      void
      remove(typename T::K const& k) = 0;
      /// Recursively indicate the nodules that the value block (i.e referenced
      /// by a quill) responsible for the given key _k_ has been modified.
      ///
      /// The nodules on the way down to the quill should make sure no limit
      /// has been reached. Otherwise, the tree structure should be adapted
      /// accordingly.
      virtual
      void
      update(typename T::K const& k) = 0;
      /// Split the current nodule and return the handle on the new nodule.
      ///
      /// Note that the nodule returned is supposed to contain the inlets with
      /// the highest keys.
      virtual
      Handle
      split() = 0;
      /// Merge the current nodule with the nodule reference by its handle
      /// _other_.
      virtual
      void
      merge(Handle& other) = 0;
      /// XXX
      virtual
      elle::Boolean
      empty() const = 0;
      /// XXX
      virtual
      elle::Boolean
      single() const = 0;
      /// XXX
      virtual
      Handle
      search(typename T::K const& k) = 0;
      /// XXX
      virtual
      Handle
      seek(Capacity const target,
           Capacity& base) = 0;
      /// XXX
      virtual
      void
      check(Flags const flags = flags::none) = 0;
      /// XXX
      virtual
      void
      seal(cryptography::SecretKey const& secret) = 0;
      /// XXX
      virtual
      void
      destroy() = 0;
      /// XXX
      virtual
      void
      dump(elle::Natural32 const margin = 0) = 0;
      /// XXX
      virtual
      void
      statistics(Statistics& stats) = 0;
      /// XXX[careful not to unload the nodule because reference is returned]
      virtual
      typename T::K const&
      mayor() const = 0;
      /// XXX[careful not to unload the nodule because reference is returned]
      virtual
      typename T::K const&
      maiden() const = 0;

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Nodule);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Nodule);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type, type);
      ELLE_ATTRIBUTE_RW(Capacity, capacity);

      /*----------.
      | Operators |
      `----------*/
    public:
      friend
      std::ostream&
      operator <<(std::ostream& stream,
                  Type const type)
      {
        switch (type)
          {
          case Type::seam:
            {
              stream << "seam";
              break;
            }
          case Type::quill:
            {
              stream << "quill";
              break;
            }
          default:
            {
              throw Exception("unknown type: '%s'", static_cast<int>(type));
            }
          }

        return (stream);
      }
    };
  }
}

# include <nucleus/proton/Nodule.hxx>

#endif

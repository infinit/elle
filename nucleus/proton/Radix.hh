#ifndef NUCLEUS_PROTON_RADIX_HH
# define NUCLEUS_PROTON_RADIX_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <nucleus/proton/Capacity.hh>
# include <nucleus/proton/fwd.hh>

namespace nucleus
{
  namespace proton
  {
    /// Represent the root/base i.e radix of a data structure for representing
    /// content in a flexible way being either directly with a value, through a
    /// single block or via a far more complex data structure i.e a tree of
    /// encrypted blocks.
    ///
    /// Note that a radix does not depend on the type of the content being
    /// represented. In order to manipulate such a content, one must
    /// instanciate a Porcupine based on the radix. This type-independence
    /// is required because Object blocks must be retrievable and explorable
    /// without knowing the type of the data represented i.e data, catalog or
    /// reference.
    class Radix:
      public elle::Printable
    {
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      /// Define the mode in which the content is represented being
      /// either empty, directly through a value, through a single
      /// block of data or via a complete tree of blocks.
      enum class Mode
        {
          empty,
          value,
          block,
          tree
        };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Radix();
      Radix(Radix const& other);

      /*--------.
      | Methods |
      `--------*/
    public:
      Value const&
      value() const;
      Value&
      value();
      Handle const&
      block() const;
      Handle&
      block();
      Tree const&
      tree() const;
      Tree&
      tree();

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Radix);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Radix);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Mode, mode);
      ELLE_ATTRIBUTE_R(Capacity, capacity);
      union
      {
        /// Represent a value being directly serialized withing the containing
        /// block.
        ///
        /// This possibility is extremely interesting for blocks, such as
        /// Object, which could be optimised so as to directly embed a value,
        /// say Data, up to a certain size after which it becomes a block
        /// which will be referenced in the Object through a handle.
        Value* _value;
        /// Represent a single block referenced through a handle.
        Handle* _block;
        /// Represent a hierarchy of blocks forming a tree-based data structure.
        Tree* _tree;
      };
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Radix::Mode const mode);
  }
}

# include <nucleus/proton/Radix.hxx>

#endif

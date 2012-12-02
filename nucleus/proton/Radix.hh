#ifndef NUCLEUS_PROTON_RADIX_HH
# define NUCLEUS_PROTON_RADIX_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Mode.hh>

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
      | Construction |
      `-------------*/
    public:
      /// Construct an empty radix.
      Radix();
      /// Construct a value-based radix.
      Radix(T* value);
      /// Construct a radix based on a single block whose address is _address_.
      Radix(Address const& address);
      /// Construct a tree-based radix given the root _root_.
      Radix(Root const& root);
      /// Copy constructor.
      Radix(Radix const& other);
      /// Destructor.
      ~Radix();

      /*--------.
      | Methods |
      `--------*/
    public:
      Value const*
      value() const;
      Value*
      value();
      Address const&
      block() const;
      Address&
      block();
      Root const&
      tree() const;
      Root&
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
      union
      {
        /// Represent a value being directly serialized withing the containing
        /// block.
        ///
        /// This possibility is extremely interesting for blocks, such as
        /// Object, which could be optimised so as to directly embed a value,
        /// say Data, up to a certain size after which it becomes a block
        /// which will be referenced in the Object through an address.
        Value* _value; // XXX[shared_ptr?]
        /// Represent a single block referenced through an address.
        Address* _address;
        /// Represent the type-independent root of a hierarchy of blocks
        /// forming a tree-based data structure.
        Root* _root;
      };
    };
  }
}

# include <nucleus/proton/Radix.hxx>

#endif

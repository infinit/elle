#ifndef NUCLEUS_PROTON_BASE_HH
# define NUCLEUS_PROTON_BASE_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>
# include <elle/cryptography/Digest.hh>
# include <elle/cryptography/oneway.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Revision.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    /// A base references a precise revision of a mutable block, often
    /// a previous revision.
    ///
    /// This construct is useful to make sure a mutable block derives
    /// from another one, and another one down to the original mutable
    /// block whose ownership can usually be statically verified.
    ///
    /// By following this chain, one can make sure a mutable block lies
    /// in the legitimate block's history, in other words, branches have
    /// not been created.
    class Base:
      public elle::Printable
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Algorithms
      {
        static const elle::cryptography::oneway::Algorithm oneway;
      };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Base();
      Base(MutableBlock const& block);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Returns true if the given block matches the base.
      elle::Boolean
      matches(MutableBlock const& block) const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Base const& other) const;
      ELLE_OPERATOR_NEQ(Base);
      ELLE_OPERATOR_ASSIGNMENT(Base); // XXX

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Base);
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Revision, revision);
      ELLE_ATTRIBUTE_R(elle::cryptography::Digest, digest);
    };

  }
}

# include <nucleus/proton/Base.hxx>

#endif

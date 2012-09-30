#ifndef NUCLEUS_PROTON_REVISION_HH
# define NUCLEUS_PROTON_REVISION_HH

# include <elle/Printable.hh>
# include <elle/idiom/Open.hh>
# include <elle/types.hh>
# include <elle/operator.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a revision number which are used to distinguish
    /// the revisions related to a mutable block.
    ///
    class Revision:
      public elle::Printable
    {
    public:
      //
      // types
      //
      typedef elle::Natural64           Type;

      //
      // constants
      //
      static const Revision              First;
      static const Revision              Last;

      static const Revision              Any;
      static const Revision&             Some;

      //
      // constructors & destructors
      //
      Revision();
      Revision(const Type);

      //
      // methods
      //
      elle::Status              Create(const Type);

      //
      // operators
      //
      elle::Boolean
      operator ==(Revision const& other) const;
      elle::Boolean
      operator <(Revision const& other) const;
      elle::Boolean
      operator >(const Revision&) const;
      Revision&
      operator +=(elle::Natural32 const increment);
      Revision
      operator +(Revision const& other) const;
      ELLE_OPERATOR_NEQ(Revision);
      ELLE_OPERATOR_LTE(Revision);
      ELLE_OPERATOR_GTE(Revision);
      ELLE_OPERATOR_ASSIGNMENT(Revision);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& s) const;

      //
      // attributes
      //
      Type                      number;
    };
  }
}

# include <nucleus/proton/Revision.hxx>

#endif

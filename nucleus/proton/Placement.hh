#ifndef NUCLEUS_PROTON_PLACEMENT_HH
# define NUCLEUS_PROTON_PLACEMENT_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/io/Dumpable.hh>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// XXX
    ///
    class Placement:
      public elle::Printable,
      public elle::io::Dumpable
    {
    public:
      //
      // constants
      //
    public:
      static const elle::Natural64      Zero;

      static const Placement            Null;

      //
      // static methods
      //
    public:
      /// XXX
      static
      Placement
      generate();

      //
      // static attribute
      //
    public:
      static elle::Natural64 counter;

      //
      // constructors & destructors
      //
    public:
      Placement();
      Placement(const elle::Natural64);

      //
      // methods
      //
    public:
      elle::Natural64
      number() const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Placement const& other) const;
      elle::Boolean
      operator <(Placement const& other) const;
      elle::Boolean
      operator <=(Placement const& other) const;
      ELLE_OPERATOR_NEQ(Placement);
      ELLE_OPERATOR_GT(Placement);
      ELLE_OPERATOR_GTE(Placement);
      ELLE_OPERATOR_ASSIGNMENT(Placement);

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

      //
      // attributes
      //
    private:
      elle::Natural64 _number;
    };

  }
}

# include <nucleus/proton/Placement.hxx>

#endif

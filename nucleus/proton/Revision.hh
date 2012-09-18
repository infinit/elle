#ifndef NUCLEUS_PROTON_REVISION_HH
# define NUCLEUS_PROTON_REVISION_HH

# include <elle/Printable.hh>
# include <elle/idiom/Open.hh>
# include <elle/radix/Object.hh>
# include <elle/types.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a revision number which are used to distinguish
    /// the revisions related to a mutable block.
    ///
    class Revision:
      public elle::radix::Object,
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
      // interfaces
      //

      // object
      declare(Revision);
      elle::Boolean             operator==(const Revision&) const;
      elle::Boolean             operator<(const Revision&) const;
      elle::Boolean             operator>(const Revision&) const;
      Revision&                  operator+=(const elle::Natural32);
      Revision                   operator+(const Revision&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Type                      number;

    /*----------.
    | Printable |
    `----------*/

    public:
      virtual
      void
      print(std::ostream& s) const;
    };
  }
}

# include <nucleus/proton/Revision.hxx>

#endif

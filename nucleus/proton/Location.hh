#ifndef NUCLEUS_PROTON_LOCATION_HH
# define NUCLEUS_PROTON_LOCATION_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Revision.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a storage location both in space and time.
    ///
    /// indeed, a location is composed of the address of the object but
    /// also the number of the object's revision which it relates to.
    ///
    class Location:
      public elle::Printable
    {
    public:
      //
      // constants
      //
      static const Location             Null;

      //
      // constructors & destructors
      //
      Location();

      //
      // methods
      //
      elle::Status      Create(const Address&,
                               const Revision&);

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Location const& other) const;
      ELLE_OPERATOR_NEQ(Location);
      ELLE_OPERATOR_ASSIGNMENT(Location);

      //
      // interfaces
      //
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
      Address           address;
      Revision           revision;
    };

  }
}

#include <nucleus/proton/Location.hxx>

#endif

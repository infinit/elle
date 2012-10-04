#ifndef NUCLEUS_PROTON_LOCATION_HH
# define NUCLEUS_PROTON_LOCATION_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Revision.hh>

namespace nucleus
{
  namespace proton
  {

    /// This class represents a storage location both in space and time.
    ///
    /// Indeed, a location is composed of the address of the block along
    /// with the block (i.e mutable) revision. This way, given a location,
    /// one can retrieve the exact content (i.e revision) of a block, which
    /// is not the case with the address alone.
    class Location:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Location(); // XXX[should no longer be used: check etoile::wall etc.]
      Location(Address const& address,
               Revision const& revision);

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Location const& other) const;
      ELLE_OPERATOR_NEQ(Location);
      ELLE_OPERATOR_ASSIGNMENT(Location);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Location);
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
      ELLE_ATTRIBUTE_R(Address, address);
      ELLE_ATTRIBUTE_R(Revision, revision);
    };

  }
}

# include <nucleus/proton/Location.hxx>

#endif

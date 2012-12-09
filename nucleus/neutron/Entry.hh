#ifndef NUCLEUS_NEUTRON_ENTRY_HH
# define NUCLEUS_NEUTRON_ENTRY_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/Footprint.hh>
# include <nucleus/proton/Address.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a directory i.e catalog entry which is
    /// composed of a name and its object's corresponding address.
    ///
    class Entry:
      public elle::Printable
    {
    public:
      //
      // constants
      //
      static const Entry                Null;

      //
      // types
      //
      typedef elle::String              Symbol;

      //
      // constructors & destructors
      //
    public:
      Entry();
      Entry(const elle::String&,
            const proton::Address&);

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Entry const& other) const;
      ELLE_OPERATOR_NEQ(Entry);
      ELLE_OPERATOR_ASSIGNMENT(Entry);

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
      // rangeable
      virtual
      elle::String const&
      symbol() const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Entry);

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE_RW(elle::String, name);
      ELLE_ATTRIBUTE_R(proton::Address, address);
      ELLE_ATTRIBUTE_RW(proton::Footprint, footprint);
    };

  }
}

# include <nucleus/neutron/Entry.hxx>

#endif

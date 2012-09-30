#ifndef NUCLEUS_NEUTRON_ATTRIBUTES_HH
# define NUCLEUS_NEUTRON_ATTRIBUTES_HH

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Trait.hh>

# include <elle/idiom/Open.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class contains a list of (key, data) tuples that can be
    /// used for storing object-specific extra information.
    ///
    class Attributes:
      public elle::Printable
      // XXX[when Object::Administrate() no longer sets the attributes but
      //     they are set separately instead]
      // private boost::noncopyable
    {
    public:
      //
      // methods
      //
      elle::Status      Add(Trait*);
      elle::Status      Exist(const elle::String&) const;
      elle::Status      Lookup(const elle::String&,
                               Trait const*&) const;
      elle::Status      Consult(const Index&,
                                const Size&,
                                Range<Trait>&) const;
      elle::Status      Update(const elle::String&,
                               const elle::String&);
      elle::Status      Remove(const elle::String&);
      elle::Status      Capacity(Size&) const;

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Attributes const& other) const;
      ELLE_OPERATOR_NEQ(Attributes);

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
      Range<Trait>      range;
    };

  }
}

# include <nucleus/neutron/Attributes.hxx>

#endif

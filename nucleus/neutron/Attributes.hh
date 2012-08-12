#ifndef NUCLEUS_NEUTRON_ATTRIBUTES_HH
# define NUCLEUS_NEUTRON_ATTRIBUTES_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Trait.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class contains a list of (key, data) tuples that can be
    /// used for storing object-specific extra information.
    ///
    class Attributes:
      public elle::radix::Object
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
      // interfaces
      //

      // object
      declare(Attributes);
      elle::Boolean     operator==(const Attributes&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Range<Trait>      range;
    };

  }
}

# include <nucleus/neutron/Attributes.hxx>

#endif

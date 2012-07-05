#ifndef NUCLEUS_NEUTRON_CATALOG_HH
# define NUCLEUS_NEUTRON_CATALOG_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/Entry.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents the content of a directory and is composed
    /// of a set of tuples (name, address).
    ///
    /// note that the Catalog does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    class Catalog:
      public elle::radix::Object
    {
    public:
      //
      // types
      //
      typedef elle::String                      K;

      //
      // constants
      //
      static const Component                    S;
      static const Component                    Q;

      //
      // constructors & destructors
      //
      Catalog(proton::Contents<Catalog>&);

      //
      // methods
      //
      elle::Status      Create();

      elle::Status      Add(Entry*);
      elle::Status      Exist(const elle::String&) const;
      elle::Status      Lookup(const elle::String&,
                               Entry*&) const;
      elle::Status      Consult(const Index&,
                                const Size&,
                                Range<Entry>&) const;
      elle::Status      Rename(const elle::String&,
                               const elle::String&);
      elle::Status      Remove(const elle::String&);
      elle::Status      Capacity(Offset&) const;

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      proton::Contents<Catalog>&        contents;

      Range<Entry>                      range;
    };

  }
}

#include <nucleus/neutron/Catalog.hxx>

#endif

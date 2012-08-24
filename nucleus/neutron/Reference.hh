#ifndef NUCLEUS_NEUTRON_REFERENCE_HH
# define NUCLEUS_NEUTRON_REFERENCE_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/fwd.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a link-specific content.
    ///
    /// note that the Reference does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    class Reference:
      public elle::radix::Object
    {
      //
      // constants
      //
    public:
      static const Component component = ComponentReference;

    public:
      //
      // constructors & destructors
      //
      Reference(proton::Contents<Reference>&);

      //
      // methods
      //
      elle::Status      Create();

      elle::Status      Bind(const elle::String&);
      elle::Status      Resolve(elle::String&) const;

      elle::Status      Capacity(Size&) const;

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      proton::Contents<Reference>&      contents;

      elle::String                      target;
    };

  }
}

#include <nucleus/neutron/Reference.hxx>

#endif

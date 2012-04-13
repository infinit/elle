//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon jul 27 10:19:21 2009]
//

#ifndef NUCLEUS_NEUTRON_REFERENCE_HH
#define NUCLEUS_NEUTRON_REFERENCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/neutron/Offset.hh>
#include <nucleus/neutron/Size.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- forward declarations --------------------------------------------
//

    template <typename T>
    class Contents;

  }

  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a link-specific content.
    ///
    /// note that the Reference does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    class Reference:
      public elle::Object
    {
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

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      proton::Contents<Reference>&      contents;

      elle::String                      target;
    };

  }
}

#endif

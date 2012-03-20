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

#include <elle/Elle.hh>

#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/State.hh>

#include <nucleus/neutron/Offset.hh>
#include <nucleus/neutron/Size.hh>

namespace nucleus
{
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

      // XXX
      proton::Contents<Reference>& contents;
      Reference(proton::Contents<Reference>& contents): contents(contents) {}

      //
      // methods
      //

      // XXX[to remove]
      elle::Status Create() { elle::StatusOk; }

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
      proton::State     state;
      elle::String      target;
    };

  }
}

#endif

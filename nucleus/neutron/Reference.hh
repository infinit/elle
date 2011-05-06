//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Reference.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [thu may  5 14:04:27 2011]
//

#ifndef NUCLEUS_NEUTRON_REFERENCE_HH
#define NUCLEUS_NEUTRON_REFERENCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/neutron/State.hh>
#include <nucleus/neutron/Offset.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/State.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the contents of a link.
    ///
    class Reference:
      public elle::Object
    {
    public:
      //
      // constructors & destructors
      //
      Reference();

      //
      // methods
      //
      elle::Status	Bind(const elle::String&);
      elle::Status	Resolve(elle::String&) const;

      elle::Status	Capacity(Size&) const;

      //
      // interfaces
      //

      // object
      declare(Reference);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      State		state;

      elle::String	target;
    };

  }
}

#endif

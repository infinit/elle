//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien.quintard   [sun sep 25 08:27:15 2011]
//

#ifndef NUCLEUS_PROTON_INLET_HH
#define NUCLEUS_PROTON_INLET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    /// XXX Address::Some est utilisee pour avoir un footprint equivalent
    ///     a une adresse valide; Address::Null a une petite empreinte
    ///     a noter que ceci est possible car un inlet aura toujours une
    ///     addresse differente de null. ce qui n'est poas le cas de nodule
    ///     pour parent, left, right.
    ///
    template <typename V,
	      typename T>
    class Inlet:
      public elle::Object
    {
    public:
      //
      // constructors & destructors
      //
      Inlet();
      Inlet(const typename V::K&,
	    T*);
      Inlet(const typename V::K&,
	    const Address&);
      ~Inlet();

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status		Serialize(elle::Archive&) const;
      elle::Status		Extract(elle::Archive&);

      //
      // attributes
      //
      typename V::K		key;
      Address			value;

      elle::Footprint		_footprint;
      T*			_value;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Inlet.hxx>

#endif

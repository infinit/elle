//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Reference.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [fri may 28 17:48:34 2010]
//

#ifndef ETOILE_KERNEL_REFERENCE_HH
#define ETOILE_KERNEL_REFERENCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/State.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Size.hh>
#include <etoile/kernel/State.hh>

#include <etoile/path/Way.hh>

namespace etoile
{
  namespace kernel
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
      elle::Status	Bind(const path::Way&);
      elle::Status	Resolve(path::Way&) const;

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

      path::Way		target;
    };

  }
}

#endif

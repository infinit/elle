//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sun sep 25 08:27:15 2011]
//

#ifndef NUCLEUS_PROTON_INLET_HH
#define NUCLEUS_PROTON_INLET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/proton/Address.hh>
#include <XXX/Handle.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a nodule item being for seams or quills.
    ///
    /// an inlet therefore embeds the item's key along with the address
    /// of the value whose type depends upon the nodule's nature. indeed,
    /// while seams' entries reference other nodules, i.e sub-seams or quills,
    /// quills' entries reference the final value.
    ///
    /// as said above, every inlet embeds the address of the value object.
    /// note however that should the inlet just be created (common case),
    /// the address would be null, hence empty. unfortunately, the address
    /// will eventually be computed. since the size of the address differs
    /// between its creation and its sealing, the footprint calculation is
    /// biaised and thus incorrect.
    ///
    /// in order to circumvent this issue, the initial empty address is set
    /// to Address::Some, whose role is to provide an address with a valid
    /// size.
    ///
    template <typename V,
              typename T>
    class Inlet:
      public elle::radix::Object
    {
    public:
      //
      // constructors & destructors
      //
      Inlet();
      Inlet(const typename V::K&,
            Handle&);

      //
      // interfaces
      //

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status              Serialize(elle::Archive&) const;
      //elle::Status              Extract(elle::Archive&);

      //
      // attributes
      //
      typename V::K             key;
      Handle                    value;

      elle::Footprint           footprint;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <XXX/Inlet.hxx>

#endif

//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [wed mar 31 23:32:06 2010]
//

#ifndef NUCLEUS_NEUTRON_ATTRIBUTES_HH
#define NUCLEUS_NEUTRON_ATTRIBUTES_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/neutron/Trait.hh>
#include <nucleus/neutron/Index.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Range.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

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
                               Trait*&) const;
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

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Range<Trait>      range;
    };

  }
}

#endif

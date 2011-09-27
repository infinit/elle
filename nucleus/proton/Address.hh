//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon feb 16 21:13:00 2009]
//

#ifndef NUCLEUS_PROTON_ADDRESS_HH
#define NUCLEUS_PROTON_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Family.hh>

#include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// this class is one of the most fundamental of the nucleus library
    /// as it is used to identify a block in the storage layer.
    ///
    /// note that this class includes a digest which is used so as to
    /// map an address onto a network's node. indeed, every node is identified
    /// by a label which happen to be a digest as well.
    ///
    /// however the address class includes additional information such
    /// as the family---i.e the type of physical block---and the
    /// component---i.e the type of logical block.
    ///
    /// noteworthy is that the family and component are also included in
    /// the generation of the address' digest.
    ///
    class Address:
      public elle::Object,
      public elle::Fileable<elle::FormatBase64>
    {
    public:
      //
      // constants
      //
      static const Address	Null;
      static Address		Vacuum;

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      //
      // constructors & destructors
      //
      Address();
      Address(const Address&);
      ~Address();

      //
      // methods
      //
      template <typename T,
		typename... TT>
      elle::Status		Create(const Family&,
				       const neutron::Component&,
				       const T&,
				       const TT&...);

      //
      // interfaces
      //

      // object
      declare(Address);
      elle::Boolean		operator==(const Address&) const;
      elle::Boolean		operator<(const Address&) const;

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status		Serialize(elle::Archive&) const;
      elle::Status		Extract(elle::Archive&);

      //
      // attributes
      //
      Family			family;
      neutron::Component	component;

      elle::Digest*		digest;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Address.hxx>

#endif

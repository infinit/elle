//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu apr 29 19:34:10 2010]
//

#ifndef NUCLEUS_NEUTRON_USER_HH
#define NUCLEUS_NEUTRON_USER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/PublicKeyBlock.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a user block which is supposed to
    /// contain information on the associated user.
    ///
    class User:
      public proton::PublicKeyBlock
    {
    public:
      //
      // methods
      //
      elle::Status      Create(const elle::PublicKey&);

      elle::Status      Seal(const elle::PrivateKey&,
                             const Access* = NULL);

      elle::Status      Validate(const proton::Address&,
                                 const Access* = NULL) const;

      //
      // interfaces
      //

      // object
      declare(User);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Attributes        attributes;
    };

  }
}

#endif

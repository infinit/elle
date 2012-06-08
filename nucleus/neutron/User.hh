#ifndef NUCLEUS_NEUTRON_USER_HH
# define NUCLEUS_NEUTRON_USER_HH

# include <elle/concept/Serializable.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/PublicKeyBlock.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a user block which is supposed to
    /// contain information on the associated user.
    ///
    class User
      : public proton::PublicKeyBlock
      , public elle::concept::Serializable<User>
    {
    public:
      //
      // methods
      //
      elle::Status      Create(elle::cryptography::PublicKey const&);

      //
      // interfaces
      //

      // object
#include <elle/idiom/Open.hh>
      declare(User);
#include <elle/idiom/Close.hh>

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      ELLE_CONCEPT_SERIALIZABLE_METHODS(User);

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Attributes        attributes;
    };

  }
}

# include <nucleus/neutron/User.hxx>

#endif

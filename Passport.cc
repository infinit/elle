#include <hole/Label.hh>

#include <elle/io/File.hh>
#include <elle/serialize/TupleSerializer.hxx>
#include <elle/Passport.hh>
#include <lune/Lune.hh>
#include <elle/Authority.hh>

#include <cryptography/PrivateKey.hh>

namespace elle
{

  /*-------------.
  | Construction |
  `-------------*/

  Passport::Passport() {} //XXX to remove

  Passport::Passport(elle::String const& id,
                     elle::String const& name,
                     cryptography::PublicKey const& owner_K,
                     elle::Authority const& authority)
    : _id{id}
    , _name{name}
    , _owner_K{owner_K}
    , _signature{authority.k->sign(elle::serialize::make_tuple(id, owner_K))}
  {
    ELLE_ASSERT(id.size() > 0);
    ELLE_ASSERT(name.size() > 0);
    ELLE_ASSERT(this->validate(authority));
  }


  ///
  /// this method verifies the validity of the passport.
  ///
  bool
  Passport::validate(elle::Authority const& authority) const
  {
    return (authority.K().verify(this->_signature,
                                 elle::serialize::make_tuple(_id, _owner_K)));
  }

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a passport.
  ///
  void
  Passport::dump(const elle::Natural32    margin) const
  {
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Passport]" << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift << "[Id] "
              << this->_id << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift << "[Name] "
              << this->_name << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Owner K] " << this->_owner_K << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Signature] " << this->_signature << std::endl;
  }
}

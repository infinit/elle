#include <hole/Label.hh>

#include <elle/io/File.hh>
#include <elle/serialize/TupleSerializer.hxx>
#include <elle/cryptography/PrivateKey.hh>

#include <elle/Passport.hh>
#include <lune/Lune.hh>
#include <elle/Authority.hh>

namespace elle
{

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a passport.
  ///
  elle::Status          Passport::Create(const hole::Label&     label,
                                         const elle::String&    id)
  {
    // set the attributes.
    this->label = label;
    this->id = id;

    return elle::Status::Ok;
  }

  ///
  /// this method seals the passport with the authority.
  ///
  elle::Status
  Passport::Seal(elle::Authority const& authority)
  {
    // sign the pair with the authority.
    this->signature =
      authority.k->sign(elle::serialize::make_tuple(this->label, this->id));

    return elle::Status::Ok;
  }

  ///
  /// this method verifies the validity of the passport.
  ///
  elle::Status
  Passport::Validate(elle::Authority const& authority)
    const
  {
    // verify the signature.
    if (authority.K.Verify(this->signature,
                           elle::serialize::make_tuple(this->label,
                                           this->id)) == elle::Status::Error)
      escape("unable to verify the signature");

    return elle::Status::Ok;
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Passport, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a passport.
  ///
  elle::Status          Passport::Dump(const elle::Natural32    margin) const
  {
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Passport]" << std::endl;

    // dump the label.
    if (this->label.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the label");

    // dump the id.
    std::cout << alignment << elle::io::Dumpable::Shift << "[Id] "
              << this->id << std::endl;

    // dump the signature.
    if (this->signature.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the signature");

    return elle::Status::Ok;
  }
}

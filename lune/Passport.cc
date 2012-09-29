#include <hole/Label.hh>

#include <elle/io/File.hh>
#include <elle/serialize/TupleSerializer.hxx>
#include <elle/standalone/Log.hh>
#include <elle/cryptography/PrivateKey.hh>

#include <lune/Passport.hh>
#include <lune/Lune.hh>
#include <lune/Authority.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the passport files extension.
  ///
  const elle::String            Passport::Extension = ".ppt";

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
  elle::Status          Passport::Seal(const Authority&         authority)
  {
    // sign the pair with the authority.
    this->signature =
      authority.k->sign(elle::serialize::make_tuple(this->label, this->id));

    return elle::Status::Ok;
  }

  ///
  /// this method verifies the validity of the passport.
  ///
  elle::Status          Passport::Validate(const Authority&     authority)
    const
  {
    // verify the signature.
    if (authority.K.Verify(this->signature,
                           elle::serialize::make_tuple(this->label,
                                           this->id)) == elle::Status::Error)
      escape("unable to verify the signature");

    return elle::Status::Ok;
  }

  elle::io::Path
  Passport::_path()
  {
    return (elle::io::Path(Lune::Passport));
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

//
// ---------- fileable --------------------------------------------------------
//

  void
  Passport::load()
  {
    this->load(Passport::_path());
  }

  void
  Passport::store() const
  {
    this->store(Passport::_path());
  }

  void
  Passport::erase()
  {
    elle::concept::Fileable<>::erase(Passport::_path());
  }

  elle::Boolean
  Passport::exists()
  {
    return (elle::concept::Fileable<>::exists(Passport::_path()));
  }

}

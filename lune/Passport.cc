#include <hole/Label.hh>

#include <elle/io/File.hh>
#include <elle/serialize/TupleSerializer.hxx>
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
    if (authority.k->Sign(elle::serialize::make_tuple(this->label, this->id),
                          this->signature) == elle::Status::Error)
      escape("unable to sign the pair with the authority");

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

  ///
  /// this method loads a system named passport file.
  ///
  elle::Status          Passport::Load()
  {
    elle::io::Path          path;
    elle::standalone::Region        region;

    // create the path.
    if (path.Create(Lune::Passport) == elle::Status::Error)
      escape("unable to create the path");

    if (this->Load(path) == elle::Status::Error)
      escape("unable to decode the object");

    return elle::Status::Ok;
  }

  ///
  /// this method stores a system named user passport.
  ///
  elle::Status          Passport::Store() const
  {
    elle::io::Path          path;
    elle::standalone::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Passport) == elle::Status::Error)
      escape("unable to create the path");

    if (this->Store(path) == elle::Status::Error)
      escape("unable to store the object");

    return elle::Status::Ok;
  }

  ///
  /// this method erases the passport.
  ///
  elle::Status          Passport::Erase() const
  {
    elle::io::Path          path;

    // create the path.
    if (path.Create(Lune::Passport) == elle::Status::Error)
      escape("unable to create the path");

    // erase the file.
    if (elle::io::File::Erase(path) == elle::Status::Error)
      escape("unable to erase the file");

    return elle::Status::Ok;
  }

  ///
  /// this method tests the passport.
  ///
  elle::Status          Passport::Exist() const
  {
    elle::io::Path          path;

    // create the path.
    if (path.Create(Lune::Passport) == elle::Status::Error)
      escape("unable to create the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

}

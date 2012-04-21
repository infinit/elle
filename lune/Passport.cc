//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [thu aug 11 13:31:24 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Passport.hh>
#include <lune/Lune.hh>

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
    if (authority.k->Sign(this->label,
                          this->id,
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
                           this->label,
                           this->id) == elle::Status::Error)
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
    std::cout << alignment << elle::Dumpable::Shift << "[Id] "
              << this->id << std::endl;

    // dump the signature.
    if (this->signature.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the signature");

    return elle::Status::Ok;
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  //elle::Status          Passport::Serialize(elle::Archive&      archive) const
  //{
  //  // serialize the attributes.
  //  if (archive.Serialize(this->label,
  //                        this->id,
  //                        this->signature) == elle::Status::Error)
  //    escape("unable to serialize the attributes");

  //  return elle::Status::Ok;
  //}

  /////
  ///// this method extracts the object.
  /////
  //elle::Status          Passport::Extract(elle::Archive&        archive)
  //{
  //  // extract the attributes.
  //  if (archive.Extract(this->label,
  //                      this->id,
  //                      this->signature) == elle::Status::Error)
  //    escape("unable to extract the attributes");

  //  return elle::Status::Ok;
  //}

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads a system named passport file.
  ///
  elle::Status          Passport::Load()
  {
    elle::Path          path;
    elle::Region        region;

    // create the path.
    if (path.Create(Lune::Passport) == elle::Status::Error)
      escape("unable to create the path");

    // read the file's content.
    if (elle::io::File::Read(path, region) == elle::Status::Error)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(
          elle::String(reinterpret_cast<char*>(region.contents),
                       region.size),
          *this) == elle::Status::Error)
      escape("unable to decode the object");

    return elle::Status::Ok;
  }

  ///
  /// this method stores a system named user passport.
  ///
  elle::Status          Passport::Store() const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Passport) == elle::Status::Error)
      escape("unable to create the path");

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::Status::Error)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(string.c_str()),
                    string.length()) == elle::Status::Error)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::io::File::Write(path, region) == elle::Status::Error)
      escape("unable to write the file's content");

    return elle::Status::Ok;
  }

  ///
  /// this method erases the passport.
  ///
  elle::Status          Passport::Erase() const
  {
    elle::Path          path;

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
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Passport) == elle::Status::Error)
      escape("unable to create the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

}

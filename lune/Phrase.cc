//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [tue may  4 20:49:38 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Phrase.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the phrase files extension.
  ///
  const elle::String            Phrase::Extension = ".phr";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a phrase.
  ///
  elle::Status          Phrase::Create(const elle::String&      pass,
                                       const elle::String&      portal)
  {
    // assign the pass.
    this->pass = pass;

    // assigne the portal line.
    this->portal = portal;

    return elle::Status::Ok;
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this operator compares two objects.
  ///
  elle::Boolean         Phrase::operator==(const Phrase&        element) const
  {
    // check the address as this may actually be the same object.
    if (this == &element)
      return elle::Status::True;

    // compare the attributes.
    if ((this->pass != element.pass) ||
        (this->portal != element.portal))
      return elle::Status::False;

    return elle::Status::True;
  }

  ///
  /// this macro-function call generates the object.
  ///
  embed(Phrase, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the object.
  ///
  elle::Status          Phrase::Dump(const elle::Natural32      margin) const
  {
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Phrase]" << std::endl;

    std::cout << alignment << elle::Dumpable::Shift
              << "[Pass] " << this->pass << std::endl;
    std::cout << alignment << elle::Dumpable::Shift
              << "[Portal] " << this->portal << std::endl;

    return elle::Status::Ok;
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  //elle::Status          Phrase::Serialize(elle::Archive&        archive) const
  //{
  //  // serialize the attributes.
  //  if (archive.Serialize(this->pass,
  //                        this->portal) == elle::Status::Error)
  //    escape("unable to serialize the attributes");

  //  return elle::Status::Ok;
  //}

  /////
  ///// this method extracts the object.
  /////
  //elle::Status          Phrase::Extract(elle::Archive&          archive)
  //{
  //  // extract the attributes.
  //  if (archive.Extract(this->pass,
  //                      this->portal) == elle::Status::Error)
  //    escape("unable to extract the attributes");

  //  return elle::Status::Ok;
  //}

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads the phrase.
  ///
  elle::Status          Phrase::Load(const elle::String&        network)
  {
    elle::Path          path;
    elle::Region        region;

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
      escape("unable to complete the path");

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
  /// this method stores the phrase.
  ///
  elle::Status          Phrase::Store(const elle::String&       network) const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
      escape("unable to complete the path");

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
  /// this method erases the phrase.
  ///
  elle::Status          Phrase::Erase(const elle::String&       network) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
      escape("unable to complete the path");

    // erase the file.
    if (elle::io::File::Erase(path) == elle::Status::Error)
      escape("unable to erase the file");

    return elle::Status::Ok;
  }

  ///
  /// this method tests the phrase.
  ///
  elle::Status          Phrase::Exist(const elle::String&       network) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
      escape("unable to complete the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

}

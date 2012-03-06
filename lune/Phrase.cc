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

    return elle::StatusOk;
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
      return elle::StatusTrue;

    // compare the attributes.
    if ((this->pass != element.pass) ||
        (this->portal != element.portal))
      return elle::StatusFalse;

    return elle::StatusTrue;
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

    return elle::StatusOk;
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  elle::Status          Phrase::Serialize(elle::Archive&        archive) const
  {
    // serialize the attributes.
    if (archive.Serialize(this->pass,
                          this->portal) == elle::StatusError)
      escape("unable to serialize the attributes");

    return elle::StatusOk;
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status          Phrase::Extract(elle::Archive&          archive)
  {
    // extract the attributes.
    if (archive.Extract(this->pass,
                        this->portal) == elle::StatusError)
      escape("unable to extract the attributes");

    return elle::StatusOk;
  }

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
    if (path.Create(Lune::Network::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", network)) == elle::StatusError)
      escape("unable to complete the path");

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(
          elle::String(reinterpret_cast<char*>(region.contents),
                       region.size),
          *this) == elle::StatusError)
      escape("unable to decode the object");

    return elle::StatusOk;
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
    if (path.Create(Lune::Network::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", network)) == elle::StatusError)
      escape("unable to complete the path");

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::StatusError)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(string.c_str()),
                    string.length()) == elle::StatusError)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::File::Write(path, region) == elle::StatusError)
      escape("unable to write the file's content");

    return elle::StatusOk;
  }

  ///
  /// this method erases the phrase.
  ///
  elle::Status          Phrase::Erase(const elle::String&       network) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", network)) == elle::StatusError)
      escape("unable to complete the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    return elle::StatusOk;
  }

  ///
  /// this method tests the phrase.
  ///
  elle::Status          Phrase::Exist(const elle::String&       network) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%NETWORK%", network)) == elle::StatusError)
      escape("unable to complete the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      return elle::StatusFalse;

    return elle::StatusTrue;
  }

}

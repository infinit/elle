#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

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
  elle::Status          Phrase::Create(const elle::network::Port port,
                                       const elle::String& pass)
  {
    this->port = port;
    this->pass = pass;

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
      return true;

    // compare the attributes.
    if ((this->port != element.port) ||
        (this->pass != element.pass))
      return false;

    return true;
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
              << "[Port] " << std::dec << this->port << std::endl;
    std::cout << alignment << elle::Dumpable::Shift
              << "[Pass] " << this->pass << std::endl;

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

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Load(path) == elle::Status::Error)
      escape("unable to decode the object");

    return elle::Status::Ok;
  }

  ///
  /// this method stores the phrase.
  ///
  elle::Status          Phrase::Store(const elle::String&       network) const
  {
    elle::Path          path;
    elle::standalone::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Network::Phrase) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Store(path) == elle::Status::Error)
      escape("unable to store the object");
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

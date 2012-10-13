#include <lune/Phrase.hh>
#include <lune/Lune.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>
#include <elle/io/Pattern.hh>
#include <elle/standalone/Region.hh>

#include <elle/idiom/Open.hh>

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

  elle::io::Path
  Phrase::_path(elle::String const& name)
  {
    return (elle::io::Path(Lune::Phrase,
                           elle::io::Piece("%NAME%", name)));
  }

  elle::io::Path
  Phrase::_path(elle::String const& network,
                elle::String const& name)
  {
    return (elle::io::Path(Lune::Network::Phrase,
                           elle::io::Piece("%NETWORK%", network),
                           elle::io::Piece("%NAME%", name)));
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

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Port] " << std::dec << this->port << std::endl;
    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Pass] " << this->pass << std::endl;

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  void
  Phrase::load(elle::String const& name)
  {
    this->load(Phrase::_path(name));
  }

  void
  Phrase::store(elle::String const& name) const
  {
    this->store(Phrase::_path(name));
  }

  void
  Phrase::erase(elle::String const& name)
  {
    elle::concept::Fileable<>::erase(Phrase::_path(name));
  }

  elle::Boolean
  Phrase::exists(elle::String const& name)
  {
    return (elle::concept::Fileable<>::exists(Phrase::_path(name)));
  }

  void
  Phrase::load(elle::String const& network,
               elle::String const& name)
  {
    this->load(Phrase::_path(network, name));
  }

  void
  Phrase::store(elle::String const& network,
                elle::String const& name) const
  {
    this->store(Phrase::_path(network, name));
  }

  void
  Phrase::erase(elle::String const& network,
                elle::String const& name)
  {
    elle::concept::Fileable<>::erase(Phrase::_path(network, name));
  }

  elle::Boolean
  Phrase::exists(elle::String const& network,
                 elle::String const& name)
  {
    return (elle::concept::Fileable<>::exists(Phrase::_path(network, name)));
  }

}

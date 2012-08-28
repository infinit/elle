#include <elle/io/Piece.hh>
#include <elle/io/File.hh>
#include <elle/standalone/Log.hh>
#include <elle/standalone/Region.hh>

#include <lune/Set.hh>
#include <lune/Lune.hh>

#include <elle/idiom/Open.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the set files extension.
  ///
  const elle::String            Set::Extension = ".set";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method adds a locus to the set.
  ///
  elle::Status          Set::Add(const elle::network::Locus&               locus)
  {
    if (std::find(this->loci.begin(),
                  this->loci.end(),
                  locus) != this->loci.end())
      escape("this locus already exists in the set");

    this->loci.push_back(locus);

    return elle::Status::Ok;
  }

  ///
  /// this method removes a locus from the set.
  ///
  elle::Status          Set::Remove(const elle::network::Locus&            locus)
  {
    auto                iterator =
      std::find(this->loci.begin(),
                this->loci.end(),
                locus);

    if (iterator == this->loci.end())
      escape("this locus does not seem to exist in the set");

    this->loci.erase(iterator);

    return elle::Status::Ok;
  }

  elle::io::Path
  Set::_path(elle::String const& network)
  {
    return (elle::io::Path(Lune::Network::Set,
                           elle::io::Piece("%NETWORK%", network)));
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Set, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the object.
  ///
  elle::Status          Set::Dump(const elle::Natural32      margin) const
  {
    elle::String        alignment(margin, ' ');
    auto                iterator = this->loci.begin();
    auto                end = this->loci.end();

    std::cout << alignment << "[Set] #" << this->loci.size() << std::endl;

    for (; iterator != end; ++iterator)
      {
        if (iterator->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the locus");
      }

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  void
  Set::load(elle::String const& network)
  {
    elle::standalone::Region region;
    std::istringstream stream;
    elle::String element;

    // read the file's content.
    if (elle::io::File::Read(Set::_path(network), region) == elle::Status::Error)
      throw std::runtime_error("unable to read the file's content");

    // set up the stream.
    stream.str(elle::String(reinterpret_cast<char*>(region.contents),
                            region.size));

    // for every string-based locus in the string.
    while (std::getline(stream, element, ' '))
      {
        // Skip comments.
        if (!element.empty() && element[0] == '#')
          continue;

        elle::network::Locus       locus;

        // build the host locus.
        if (locus.Create(element) == elle::Status::Error)
          throw std::runtime_error("unable to create the host locus");

        // add the locus to the set.
        if (this->Add(locus) == elle::Status::Error)
          throw std::runtime_error("unable to add the locus");
      }
  }

  void
  Set::store(elle::String const& network) const
  {
    elle::io::Path path(Set::_path(network));
    std::ofstream out(path.string());

    if (!out.good())
      throw std::runtime_error("cannot open file");

    auto it = this->loci.begin();
    auto end = this->loci.end();

    for (; it != end; ++it)
      {
        elle::String host;

        // convert the current locus into a string.
        if (it->host.Convert(host) == elle::Status::Error)
          throw std::runtime_error("unable to convert the locus into a string");

        out << host << ":" << it->port << " ";
      }
  }

  void
  Set::erase(elle::String const& network)
  {
    elle::concept::Fileable<>::erase(Set::_path(network));
  }

  elle::Boolean
  Set::exists(elle::String const& network)
  {
    return (elle::concept::Fileable<>::exists(Set::_path(network)));
  }

}

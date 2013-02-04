#include <elle/io/Piece.hh>
#include <elle/io/File.hh>
#include <elle/Buffer.hh>
#include <elle/os/path.hh>

#include <common/common.hh>

#include <lune/Set.hh>
#include <lune/Lune.hh>

namespace lune
{

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
      throw elle::Exception("this locus already exists in the set");

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
      throw elle::Exception("this locus does not seem to exist in the set");

    this->loci.erase(iterator);

    return elle::Status::Ok;
  }

  std::string
  Set::_path(elle::String const& user_id,
             elle::String const& network_id)
  {
    return elle::os::path::join(
      common::infinit::network_directory(user_id, network_id),
      network_id + ".set"
    );
  }

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
          throw elle::Exception("unable to dump the locus");
      }

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  void
  Set::load(elle::String const& user,
            elle::String const& network)
  {
    std::istringstream stream;
    elle::String element;

    // read the file's content.
    elle::Buffer buffer;

    if (elle::io::File::Read(elle::io::Path{Set::_path(user, network)},
                             buffer) == elle::Status::Error)
      throw std::runtime_error("unable to read the file's content");

    // set up the stream.
    stream.str(elle::String(reinterpret_cast<char*>(buffer.mutable_contents()),
                            buffer.size()));

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
  Set::store(elle::String const& user,
             elle::String const& network) const
  {
    elle::io::Path path(Set::_path(user, network));
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
  Set::erase(elle::String const& user,
             elle::String const& network)
  {
    elle::concept::Fileable<>::erase(
      elle::io::Path{Set::_path(user, network)});
  }

  elle::Boolean
  Set::exists(elle::String const& user,
              elle::String const& network)
  {
    return (elle::concept::Fileable<>::exists(
      elle::io::Path{Set::_path(user, network)}));
  }

}

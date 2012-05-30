#include <elle/io/Piece.hh>
#include <elle/io/File.hh>

#include <lune/Set.hh>
#include <lune/Lune.hh>

#include <elle/idiom/Open.hh>

using namespace lune;

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
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes the object.
///
//elle::Status          Set::Serialize(elle::Archive&        archive) const
//{
//  elle::Natural32     size;
//  auto                iterator = this->loci.begin();
//  auto                end = this->loci.end();
//
//  size = this->loci.size();
//
//  if (archive.Serialize(size) == elle::Status::Error)
//    escape("unable to serialize the size");
//
//  for (; iterator != end; ++iterator)
//    {
//      if (archive.Serialize(*iterator) == elle::Status::Error)
//        escape("unable to serialize the attributes");
//    }
//
//  return elle::Status::Ok;
//}
//
/////
///// this method extracts the object.
/////
//elle::Status          Set::Extract(elle::Archive&          archive)
//{
//  elle::Natural32     size;
//  elle::Natural32     i;
//
//  if (archive.Extract(size) == elle::Status::Error)
//    escape("unable to extract the size");
//
//  for (i = 0; i < size; i++)
//    {
//      elle::network::Locus     locus;
//
//      if (archive.Extract(locus) == elle::Status::Error)
//        escape("unable to extract the attributes");
//
//      if (this->Add(locus) == elle::Status::Error)
//        escape("unable to add the locus to the set");
//    }
//
//  return elle::Status::Ok;
//}

//
// ---------- fileable --------------------------------------------------------
//

///
/// this method loads the set.
///
elle::Status          Set::Load(const elle::String&        network)
{
  elle::Path          path;
  elle::standalone::Region        region;
  std::istringstream  stream;
  elle::String        element;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::Status::Error)
    escape("unable to create the path");

  // complete the path's pattern.
  if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
    escape("unable to complete the path");

  // read the file's content.
  if (elle::io::File::Read(path, region) == elle::Status::Error)
    escape("unable to read the file's content");

  // set up the stream.
  stream.str(elle::String(reinterpret_cast<char*>(region.contents),
                          region.size));

  // for every string-based locus in the string.
  while (std::getline(stream, element, ' '))
    {
      elle::network::Locus       locus;

      // build the host locus.
      if (locus.Create(element) == elle::Status::Error)
        escape("unable to create the host locus");

      // add the locus to the set.
      if (this->Add(locus) == elle::Status::Error)
        escape("unable to add the locus");
    }

  return elle::Status::Ok;
}

///
/// this method stores the set.
///
elle::Status          Set::Store(const elle::String&       network) const
{
  elle::Path          path;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::Status::Error)
    escape("unable to create the path");

  // complete the path's pattern.
  if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
    escape("unable to complete the path");

  std::ofstream out(path.string);

  if (!out.good())
    escape("cannot open file %s", path.string.c_str());

  auto it = this->loci.begin(), end = this->loci.end();
  for (; it != end; ++it)
    {
      elle::String      host;

      // convert the current locus into a string.
      if (it->host.Convert(host) == elle::Status::Error)
        escape("unable to convert the locus into a string");

      out << host << ":" << it->port << " ";
    }

  return elle::Status::Ok;
}

///
/// this method erases the set.
///
elle::Status          Set::Erase(const elle::String&       network) const
{
  elle::Path          path;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::Status::Error)
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
/// this method tests the set.
///
elle::Status          Set::Exist(const elle::String&       network) const
{
  elle::Path          path;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::Status::Error)
    escape("unable to create the path");

  // complete the path's pattern.
  if (path.Complete(elle::io::Piece("%NETWORK%", network)) == elle::Status::Error)
    escape("unable to complete the path");

  // test the file.
  if (elle::io::File::Exist(path) == elle::Status::False)
    return elle::Status::False;

  return elle::Status::True;
}

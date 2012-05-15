//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [tue mar  6 15:15:47 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Set.hh>
#include <lune/Lune.hh>

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
elle::Status          Set::Add(const elle::Locus&               locus)
{
  if (std::find(this->loci.begin(),
                this->loci.end(),
                locus) != this->loci.end())
    escape("this locus already exists in the set");

  this->loci.push_back(locus);

  return elle::StatusOk;
}

///
/// this method removes a locus from the set.
///
elle::Status          Set::Remove(const elle::Locus&            locus)
{
  auto                iterator =
    std::find(this->loci.begin(),
              this->loci.end(),
              locus);

  if (iterator == this->loci.end())
    escape("this locus does not seem to exist in the set");

  this->loci.erase(iterator);

  return elle::StatusOk;
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
      if (iterator->Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the locus");
    }

  return elle::StatusOk;
}

//
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes the object.
///
elle::Status          Set::Serialize(elle::Archive&        archive) const
{
  elle::Natural32     size;
  auto                iterator = this->loci.begin();
  auto                end = this->loci.end();

  size = this->loci.size();

  if (archive.Serialize(size) == elle::StatusError)
    escape("unable to serialize the size");

  for (; iterator != end; ++iterator)
    {
      if (archive.Serialize(*iterator) == elle::StatusError)
        escape("unable to serialize the attributes");
    }

  return elle::StatusOk;
}

///
/// this method extracts the object.
///
elle::Status          Set::Extract(elle::Archive&          archive)
{
  elle::Natural32     size;
  elle::Natural32     i;

  if (archive.Extract(size) == elle::StatusError)
    escape("unable to extract the size");

  for (i = 0; i < size; i++)
    {
      elle::Locus     locus;

      if (archive.Extract(locus) == elle::StatusError)
        escape("unable to extract the attributes");

      if (this->Add(locus) == elle::StatusError)
        escape("unable to add the locus to the set");
    }

  return elle::StatusOk;
}

//
// ---------- fileable --------------------------------------------------------
//

///
/// this method loads the set.
///
elle::Status          Set::Load(const elle::String&        network)
{
  elle::Path          path;
  elle::Region        region;
  std::istringstream  stream;
  elle::String        element;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::StatusError)
    escape("unable to create the path");

  // complete the path's pattern.
  if (path.Complete(elle::Piece("%NETWORK%", network)) == elle::StatusError)
    escape("unable to complete the path");

  // read the file's content.
  if (elle::File::Read(path, region) == elle::StatusError)
    escape("unable to read the file's content");

  // set up the stream.
  stream.str(elle::String(reinterpret_cast<char*>(region.contents),
                          region.size));

  // for every string-based locus in the string.
  while (std::getline(stream, element, ' '))
    {
      elle::Locus       locus;

      // build the host locus.
      if (locus.Create(element) == elle::StatusError)
        escape("unable to create the host locus");

      // add the locus to the set.
      if (this->Add(locus) == elle::StatusError)
        escape("unable to add the locus");
    }

  return elle::StatusOk;
}

///
/// this method stores the set.
///
elle::Status          Set::Store(const elle::String&       network) const
{
  elle::Path          path;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::StatusError)
    escape("unable to create the path");

  // complete the path's pattern.
  if (path.Complete(elle::Piece("%NETWORK%", network)) == elle::StatusError)
    escape("unable to complete the path");

  std::ofstream out(path.string);

  if (!out.good())
    escape("cannot open file %s", path.string.c_str());

  auto it = this->loci.begin(), end = this->loci.end();
  for (; it != end; ++it)
    {
      elle::String      host;

      // convert the current locus into a string.
      if (it->host.Convert(host) == elle::StatusError)
        escape("unable to convert the locus into a string");

      out << host << ":" << it->port << " ";
    }

  return elle::StatusOk;
}

///
/// this method erases the set.
///
elle::Status          Set::Erase(const elle::String&       network) const
{
  elle::Path          path;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::StatusError)
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
/// this method tests the set.
///
elle::Status          Set::Exist(const elle::String&       network) const
{
  elle::Path          path;

  // create the path.
  if (path.Create(Lune::Network::Set) == elle::StatusError)
    escape("unable to create the path");

  // complete the path's pattern.
  if (path.Complete(elle::Piece("%NETWORK%", network)) == elle::StatusError)
    escape("unable to complete the path");

  // test the file.
  if (elle::File::Exist(path) == elle::StatusFalse)
    return elle::StatusFalse;

  return elle::StatusTrue;
}

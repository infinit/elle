
#include <nucleus/proton/AddressSerializer.hxx>
#include <lune/DescriptorSerializer.hxx>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <lune/Descriptor.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the descriptor files extension.
  ///
  const elle::String            Descriptor::Extension = ".dsc";

  ///
  /// this constant defines whether or not the history functionality
  /// is activated for this network.
  ///
  const elle::Boolean           Descriptor::History = false;

  ///
  /// this constant defines the size of the blocks stored within this
  /// network.
  ///
  const elle::Natural32         Descriptor::Extent = 400; // XXX 8192

  ///
  /// this constant defines how much data must stay in the porcupine
  /// nodule being split.
  ///
  /// this value can be used to prevent most nodes to be half filled
  /// especially when data is added in a single sequential manner leading
  /// to blocks getting filled and split.
  ///
  const elle::Real              Descriptor::Contention = 0.5;

  ///
  /// this constant defines the bound which, once reached, implies
  /// that data from a porcupine nodule must be balanced on neighbour nodes,
  /// if possible.
  ///
  const elle::Real              Descriptor::Balancing = 0.2;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a descriptor.
  ///
  elle::Status          Descriptor::Create(const elle::String&  name,
                                           const hole::Model&   model,
                                           const nucleus::Address& root,
                                           const elle::Boolean  history,
                                           const elle::Natural32 extent,
                                           const elle::Real&    contention,
                                           const elle::Real&    balancing)
  {
    // set the attributes.
    this->name = name;
    this->model = model;
    this->root = root;
    this->history = history;
    this->extent = extent;
    this->contention = contention;
    this->balancing = balancing;

    return elle::Status::Ok;
  }

  ///
  /// this method seals the descriptor with the authority.
  ///
  elle::Status          Descriptor::Seal(const Authority&       authority)
  {
    // sign the attributesr with the authority.
    if (authority.k->Sign(elle::serialize::make_tuple(this->name,
                                                      this->model,
                                                      this->root,
                                                      this->history,
                                                      this->extent,
                                                      this->contention,
                                                      this->balancing),
                          this->signature) == elle::Status::Error)
      escape("unable to sign the attributes with the authority");

    return elle::Status::Ok;
  }

  ///
  /// this method verifies the validity of the descriptor.
  ///
  elle::Status          Descriptor::Validate(const Authority&   authority)
    const
  {
    // verify the signature.
    if (authority.K.Verify(this->signature,
                           elle::serialize::make_tuple(this->name,
                                                       this->model,
                                                       this->root,
                                                       this->history,
                                                       this->extent,
                                                       this->contention,
                                                       this->balancing)) == elle::Status::Error)
      escape("unable to verify the signature");

    return elle::Status::Ok;
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Descriptor, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a descriptor.
  ///
  /// note that this method may actually not dump the current values of
  /// the parameters.
  ///
  elle::Status          Descriptor::Dump(const elle::Natural32  margin) const
  {
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Descriptor]" << std::endl;

    std::cout << alignment << elle::Dumpable::Shift << "[Name] "
              << this->name << std::endl;

    if (this->model.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the model");

    std::cout << alignment << elle::Dumpable::Shift << "[Root] " << std::endl;

    if (this->root.Dump(margin + 4) == elle::Status::Error)
      escape("unable to dump the address");

    std::cout << alignment << elle::Dumpable::Shift << "[History] "
              << static_cast<elle::Natural32>(this->history) << std::endl;

    std::cout << alignment << elle::Dumpable::Shift << "[Extent] "
              << this->extent << std::endl;

    std::cout << alignment << elle::Dumpable::Shift << "[Contention] "
              << this->contention << std::endl;

    std::cout << alignment << elle::Dumpable::Shift << "[Balancing] "
              << this->balancing << std::endl;

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
  //elle::Status          Descriptor::Serialize(elle::Archive&    archive) const
  //{
  //  // serialize the attributes.
  //  if (archive.Serialize(
  //        this->name,
  //        this->model,
  //        this->root,
  //        this->history,
  //        this->extent,
  //        this->contention,
  //        this->balancing,
  //        this->signature) == elle::Status::Error)
  //    escape("unable to serialize the attributes");

  //  return elle::Status::Ok;
  //}

  /////
  ///// this method extracts the object.
  /////
  //elle::Status          Descriptor::Extract(elle::Archive&      archive)
  //{
  //  // extract the attributes.
  //  if (archive.Extract(this->name,
  //                      this->model,
  //                      this->root,
  //                      this->history,
  //                      this->extent,
  //                      this->contention,
  //                      this->balancing,
  //                      this->signature) == elle::Status::Error)
  //    escape("unable to extract the attributes");

  //  return elle::Status::Ok;
  //}

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads a descriptor.
  ///
  elle::Status          Descriptor::Load(const elle::String&    name)
  {
    elle::Path          path;
    elle::standalone::Region        region;

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Load(path) == elle::Status::Error)
      escape("unable to load '%s'", path.str().c_str());

    return elle::Status::Ok;
  }

  ///
  /// this method stores a descriptor.
  ///
  elle::Status          Descriptor::Store(const elle::String&   name) const
  {
    elle::Path          path;
    elle::standalone::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Store(path) == elle::Status::Error)
      escape("unable to store '%s'", path.str().c_str());

    return elle::Status::Ok;
  }

  ///
  /// this method erases the descriptor.
  ///
  elle::Status          Descriptor::Erase(const elle::String&   name) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    // erase the file.
    if (elle::io::File::Erase(path) == elle::Status::Error)
      escape("unable to erase the file");

    return elle::Status::Ok;
  }

  ///
  /// this method tests the descriptor.
  ///
  elle::Status          Descriptor::Exist(const elle::String&   name) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

}

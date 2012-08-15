#include <lune/Descriptor.hh>
#include <lune/Authority.hh>
#include <lune/Lune.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>
#include <elle/cryptography/PrivateKey.hh>
#include <elle/standalone/Log.hh>

#include <nucleus/proton/Address.hh>

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
  elle::Status
  Descriptor::Create(const elle::String id,
                     const elle::String&  name,
                     const hole::Model&   model,
                     hole::Openness const& openness,
                     const nucleus::proton::Address& root,
                     nucleus::neutron::Group::Identity const& everybody,
                     const elle::Boolean  history,
                     const elle::Natural32 extent,
                     const elle::Real&    contention,
                     const elle::Real&    balancing,
                     horizon::Policy const& policy)
  {
    // set the attributes.
    this->_id = id;
    this->name = name;
    this->model = model;
    this->_openness = openness;
    this->root = root;
    this->everybody = everybody;
    this->history = history;
    this->extent = extent;
    this->contention = contention;
    this->balancing = balancing;
    this->_policy = policy;

    return elle::Status::Ok;
  }

  ///
  /// this method seals the descriptor with the authority.
  ///
  elle::Status          Descriptor::Seal(const Authority&       authority)
  {
    // sign the attributesr with the authority.
    if (authority.k->Sign(
          elle::serialize::make_tuple(
            this->_id,
            this->name,
            this->model,
            this->_openness,
            this->root,
            this->everybody,
            this->history,
            this->extent,
            this->contention,
            this->balancing,
            this->_policy),
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
    if (authority.K.Verify(
          this->signature,
          elle::serialize::make_tuple(
            this->_id,
            this->name,
            this->model,
            this->_openness,
            this->root,
            this->everybody,
            this->history,
            this->extent,
            this->contention,
            this->balancing,
            this->_policy)) == elle::Status::Error)
      escape("unable to verify the signature");

    return elle::Status::Ok;
  }

  elle::String const&
  Descriptor::id() const
  {
    return (this->_id);
  }

  void
  Descriptor::id(elle::String const& id)
  {
    this->_id = id;
  }

  hole::Openness const&
  Descriptor::openness() const
  {
    return (this->_openness);
  }

  horizon::Policy const&
  Descriptor::policy() const
  {
    return (this->_policy);
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
    elle::io::Unique unique;

    std::cout << alignment << "[Descriptor]" << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift << "[id] "
              << this->_id << std::endl
              << alignment << elle::io::Dumpable::Shift << "[Name] "
              << this->name << std::endl;

    if (this->model.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the model");

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Openness] " << this->_openness << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Root] " << std::endl;

    if (this->root.Dump(margin + 4) == elle::Status::Error)
      escape("unable to dump the address");

    if (this->everybody.Save(unique) == elle::Status::Error)
      escape("unable to save the address");

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Everybody] " << unique << std::endl;

    if (this->everybody.Dump(margin + 4) == elle::Status::Error)
      escape("unable to dump the address");

    std::cout << alignment << elle::io::Dumpable::Shift << "[History] "
              << static_cast<elle::Natural32>(this->history) << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift << "[Extent] "
              << this->extent << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift << "[Contention] "
              << this->contention << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift << "[Balancing] "
              << this->balancing << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Policy] " << this->_policy << std::endl;

    if (this->signature.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the signature");

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads a descriptor.
  ///
  elle::Status          Descriptor::Load(const elle::String&    name)
  {
    elle::io::Path          path;
    elle::standalone::Region        region;

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Load(path) == elle::Status::Error)
      escape("unable to load '%s'", path.string().c_str());

    return elle::Status::Ok;
  }

  ///
  /// this method stores a descriptor.
  ///
  elle::Status          Descriptor::Store(const elle::String&   name) const
  {
    elle::io::Path          path;
    elle::standalone::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Network::Descriptor) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Store(path) == elle::Status::Error)
      escape("unable to store '%s'", path.string().c_str());

    return elle::Status::Ok;
  }

  ///
  /// this method erases the descriptor.
  ///
  elle::Status          Descriptor::Erase(const elle::String&   name) const
  {
    elle::io::Path          path;

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
    elle::io::Path          path;

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

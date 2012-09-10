#include <lune/Descriptor.hh>
#include <lune/Authority.hh>
#include <lune/Lune.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>
#include <elle/cryptography/PrivateKey.hh>
#include <elle/standalone/Log.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/neutron/Subject.hh>

#include <hole/Openness.hh>

#include <elle/idiom/Open.hh>

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
// ---------- construction ----------------------------------------------------
//

  Descriptor::Descriptor()
  {
    this->_everybody.subject = nullptr;
  }

  Descriptor::~Descriptor()
  {
    delete this->_everybody.subject;
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a descriptor.
  ///
  elle::Status
  Descriptor::Create(const elle::String id,
                     Version const& version,
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
    this->_version = version;
    this->name = name;
    this->model = model;
    this->_openness = openness;
    this->root = root;
    this->_everybody.identity = everybody;
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
            this->_version,
            this->name,
            this->model,
            this->_openness,
            this->root,
            this->_everybody.identity,
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
            this->_version,
            this->name,
            this->model,
            this->_openness,
            this->root,
            this->_everybody.identity,
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

  Version const&
  Descriptor::version() const
  {
    return (this->_version);
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

  nucleus::neutron::Group::Identity const&
  Descriptor::everybody_identity() const
  {
    return (this->_everybody.identity);
  }

  nucleus::neutron::Subject const&
  Descriptor::everybody_subject()
  {
    this->_everybody_subject();

    return (*this->_everybody.subject);
  }

  void
  Descriptor::_everybody_subject()
  {
    // Create the subject corresponding to the everybody group, if necessary.
    // Note that this subject will never be serialized but is used to ease
    // the process of access control since most method manipulate subjects.
    if (this->_everybody.subject == nullptr)
      this->_everybody.subject =
        new nucleus::neutron::Subject(this->_everybody.identity);

    assert(this->_everybody.subject != nullptr);
  }

  elle::io::Path
  Descriptor::_path(elle::String const& network)
  {
    return (elle::io::Path(Lune::Network::Descriptor,
                           elle::io::Piece("%NETWORK%", network)));
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

    std::cout << alignment << elle::io::Dumpable::Shift << "[Version] "
              << this->_version << std::endl;

    if (this->model.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the model");

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Openness] " << this->_openness << std::endl;

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Root] " << std::endl;

    if (this->root.Dump(margin + 4) == elle::Status::Error)
      escape("unable to dump the address");

    if (this->_everybody.identity.Save(unique) == elle::Status::Error)
      escape("unable to save the address");

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Everybody] " << unique << std::endl;

    if (this->_everybody.identity.Dump(margin + 4) == elle::Status::Error)
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

  void
  Descriptor::load(elle::String const& network)
  {
    this->load(Descriptor::_path(network));
  }

  void
  Descriptor::store(elle::String const& network) const
  {
    this->store(Descriptor::_path(network));
  }

  void
  Descriptor::erase(elle::String const& network)
  {
    elle::concept::Fileable<>::erase(Descriptor::_path(network));
  }

  elle::Boolean
  Descriptor::exists(elle::String const& network)
  {
    return (elle::concept::Fileable<>::exists(Descriptor::_path(network)));
  }

}

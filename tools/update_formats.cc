#include <lune/Authority.hh>
#include <lune/Descriptor.hh>
#include <lune/Identity.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Record.hh>

#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/Base64Archive.hh>
#include <elle/serialize/VectorSerializer.hxx>
#include <elle/serialize/PairSerializer.hxx>

#include <iostream>
#include <fstream>

template <typename T>
void
add_class(std::string const& name, T const& val)
{
  std::string filename = name + ".txt";
  static unsigned int const format = elle::serialize::StaticFormat<T>::version;
  std::vector<std::pair<uint32_t, std::string>> lines;
  {
    std::ifstream in(filename);
    if (in.good())
      {
        elle::serialize::InputBinaryArchive(in) >> lines;
        std::cout << "Loaded " << lines.size()
                  << " format of class " << name << std::endl;
      }
    else
      std::cout << "Creating " << filename << " for class "
                << name << std::endl;
  }

  for (auto const& pair: lines)
    if (pair.first == format)
      {
        std::cout << "Current format " << format << " of class " << name
                  << " is already present" << std::endl;
        return;
      }

  std::cout << "Saving (new) current format " << format
            << " of class " << name << std::endl;

  std::string str;
  {
    std::ostringstream ss;
    elle::serialize::OutputBase64Archive(ss) << val;
    str = ss.str();
  }

  lines.push_back(std::make_pair(format, str));

  {
    std::ofstream out(filename);
    if (!out.good())
      throw std::runtime_error{"Cannot open '" + filename + "'"};

    elle::serialize::OutputBinaryArchive(out) << lines;
  }
}

struct Authority: lune::Authority
{
  Authority()
  {

    elle::cryptography::KeyPair pair;
    if (pair.Generate() == elle::Status::Error)
      throw std::runtime_error("unable to generate the key pair");
    this->Create(pair);
    this->Encrypt("authority password");
  }

};

struct Identity:
  lune::Identity
{
  Identity(lune::Authority& auth,
           std::string const& id,
           std::string const& login,
           std::string const& password)
  {
    elle::cryptography::KeyPair pair;
    if (pair.Generate() == elle::Status::Error)
      throw std::runtime_error("unable to generate the key pair");

    if (this->Create(id, login, pair) == elle::Status::Error)
      throw std::runtime_error("unable to create the identity");

    if (this->Encrypt(password) == elle::Status::Error)
      throw std::runtime_error("unable to encrypt the identity");

    if (this->Seal(auth) == elle::Status::Error)
      throw std::runtime_error("unable to seal the identity");
  }
};


struct Group
{
  nucleus::neutron::Group     block;
  nucleus::proton::Address    address;
  nucleus::neutron::Subject   subject;

  Group(lune::Identity& identity, std::string const& name):
    block{identity.pair.K, name}
  {
    this->block.seal(identity.pair.k);

    if (this->block.Bind(this->address) == elle::Status::Error)
      throw std::runtime_error("unable to bind the group");

    if (this->subject.Create(this->address) == elle::Status::Error)
      throw std::runtime_error("unable to create the group subject");
  }
};


struct Access
{
  nucleus::neutron::Access  block;
  nucleus::proton::Address  address;

  Access(Group& group)
  {
    auto res = this->block.Add(new nucleus::neutron::Record(
        group.subject,
        nucleus::neutron::PermissionRead,
        nucleus::neutron::Token::Null
    ));
    if (res == elle::Status::Error)
      throw std::runtime_error("unable to add the record to the access");
    if (this->block.Bind(this->address) == elle::Status::Error)
      throw std::runtime_error("unable to bind the access");
  }
};


struct Directory
{
  nucleus::neutron::Object  block;
  nucleus::proton::Address  address;

  Directory(lune::Identity& identity, Access& access)
  {
    if (this->block.Create(nucleus::neutron::GenreDirectory,
                           identity.pair.K) == elle::Status::Error)
      throw std::runtime_error("Couldn't create the root block.");
    if (this->block.Update(block.author(),
                           this->block.contents(),
                           this->block.size(),
                           access.address,
                           this->block.owner_token()) == elle::Status::Error)
      throw std::runtime_error("unable to update the directory");
    if (this->block.Seal(identity.pair.k, access.block) == elle::Status::Error)
      throw std::runtime_error("Cannot seal the access");
    if (this->block.Bind(this->address) == elle::Status::Error)
      throw std::runtime_error("Couldn't bind the address.");
  }
};

#include <elle/idiom/Close.hh>

#include <boost/program_options.hpp>

# define ADD_CLASS(__type, var) \
  add_class<__type>(#__type, var) \
/**/

static void _initAll();

int main(int ac, char** av)
{
  _initAll();

  std::string authority_path;

  namespace po = boost::program_options;
  po::options_description description{"Update format versionning test files."};
  description.add_options()
    ("help,h", "Print this message")
    (
      "authority-path",
      po::value<std::string>(&authority_path)
        ->default_value("../oracle/infinit.auth"),
      "Path to the authority file"
    )
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, description), vm);
  po::notify(vm);

  if (vm.count("help"))
    {
      std::cerr << description << std::endl;
      return 1;
    }


  Authority auth;
  ADD_CLASS(lune::Authority, auth);

  Identity identity(auth, "identity id", "login@example.com", "football");
  ADD_CLASS(lune::Identity, identity);

  Group group(identity, "everybody");
  ADD_CLASS(nucleus::neutron::Group, group.block);
  ADD_CLASS(nucleus::neutron::Subject, group.subject);

  Access access(group);
  ADD_CLASS(nucleus::neutron::Access, access.block);

  Directory directory(identity, access);
  ADD_CLASS(nucleus::neutron::Object, directory.block);

//  auto res = descriptor.Create(
//      "network id",
//      Infinit::version,
//      "network name",
//      hole::Mode::TypeSlug,
//      hole::Openness::open,
//      root_address,
//      everybody_group,
//      false,
//      4096,
//      0,
//      0,
//      horizon::Policy::accessible
//  );
//
//  assert(res == elle::Status::Ok);

  //add_class<int>("pif");
  return 0;
}

#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <nucleus/Nucleus.hh>

static void _initAll()
{
  if (elle::Elle::Initialize() == elle::Status::Error ||
      lune::Lune::Initialize() == elle::Status::Error ||
      nucleus::Nucleus::Initialize() == elle::Status::Error)
    {
      show();
      throw std::runtime_error("Couldn't initialize !");
    }
}

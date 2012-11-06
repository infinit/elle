#define BOOST_TEST_DYN_LINK
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <elle/attribute.hh>
#include <elle/cast.hh>
#include <elle/cryptography/KeyPair.hh>

#include <hole/implementations/slug/Implementation.hh>
#include <hole/storage/Directory.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Genre.hh>
#include <nucleus/neutron/Object.hh>

/*-------------------.
| TemporaryDirectory |
`-------------------*/

class TemporaryDirectory
{
public:
  TemporaryDirectory()
  {
    do
      {
        this->_path = this->_tmpdir / boost::filesystem::unique_path();
      }
    while (!boost::filesystem::create_directory(this->_path));
  }

  ~TemporaryDirectory()
  {
    boost::filesystem::remove_all(this->_path);
  }

private:
  static boost::filesystem::path _tmpdir;
  ELLE_ATTRIBUTE_R(boost::filesystem::path, path);
};

boost::filesystem::path TemporaryDirectory::_tmpdir(
  boost::filesystem::temp_directory_path());

static std::vector<elle::network::Locus> members()
{
  std::vector<elle::network::Locus> res;
  res.push_back(elle::network::Locus("127.0.0.1", 12345));
  return res;
}

elle::cryptography::KeyPair keys(elle::cryptography::KeyPair::generate());
elle::Authority authority(keys);
elle::Passport passport;

class Slug: public hole::implementations::slug::Implementation
{
public:
  Slug(elle::Passport const& passport = ::passport,
       elle::Authority const& authority = ::authority,
       std::vector<elle::network::Locus> const& members = ::members())
    : hole::implementations::slug::Implementation(
      _storage, passport, authority, members, 12345,
      boost::posix_time::seconds(1))
    , _tmp()
    , _storage(_tmp.path().native())
  {}

private:
  TemporaryDirectory _tmp;
  hole::storage::Directory _storage;
};

void
test()
{
  Slug s1;
  s1.join();
  Slug s2;
  s2.join();

  nucleus::proton::Network network("namespace");

  elle::cryptography::KeyPair user_keys(
    elle::cryptography::KeyPair::generate());

  nucleus::neutron::Object block(network, user_keys.K,
                                 nucleus::neutron::Genre::file);
  block.Update(nucleus::neutron::Author(),
               nucleus::proton::Address::null(),
               42,
               nucleus::proton::Address::null(),
               nucleus::neutron::Token::null());
  block.Seal(user_keys.k, nullptr);

  auto address = block.bind();
  s1.push(address, block);

  auto retreived = elle::cast<nucleus::neutron::Object>::runtime(
    s2.pull(address, nucleus::proton::Revision::Last));
  BOOST_CHECK(retreived);
  BOOST_CHECK_EQUAL(retreived->size(), 42);

  elle::concurrency::scheduler().terminate();
}

// At some point all slug instances used to share the same Machine instance (the
// last allocated). Check that by instantiating two slug and NOT connecting
// them, they don't see each other's blocks.
void
test_separate_missing()
{
  Slug s1(passport, authority, std::vector<elle::network::Locus>());
  s1.join();
  Slug s2(passport, authority, std::vector<elle::network::Locus>());
  s2.join();

  nucleus::proton::Network network("namespace");

  elle::cryptography::KeyPair user_keys(
    elle::cryptography::KeyPair::generate());

  nucleus::neutron::Object block(network, user_keys.K,
                                 nucleus::neutron::Genre::file);
  block.Seal(user_keys.k, nullptr);

  auto address = block.bind();
  s1.push(address, block);

  BOOST_CHECK_THROW(s2.pull(address, nucleus::proton::Revision::Last),
                    reactor::Exception);
  elle::concurrency::scheduler().terminate();
}

void
sched_wrapper(std::function<void()> const& f)
{
  auto& sched = elle::concurrency::scheduler();
  reactor::Thread test(sched, "test", f);
  sched.run();
}

#define TEST_CASE(Test)                                 \
  BOOST_TEST_CASE(std::bind(sched_wrapper, Test))       \

bool test_suite()
{
  boost::unit_test::test_suite* slug = BOOST_TEST_SUITE(
    "infinit::hole::implementations::slug");
  //slug->add(TEST_CASE(test));
  slug->add(TEST_CASE(test_separate_missing));

  boost::unit_test::framework::master_test_suite().add(slug);

  return true;
}

int
main(int argc, char** argv)
{
  passport.Seal(authority);

  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}

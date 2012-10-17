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

void test()
{
  elle::cryptography::KeyPair keys(elle::cryptography::KeyPair::generate());
  elle::Authority authority(keys);
  elle::Passport passport;
  passport.Seal(authority);

  std::vector<elle::network::Locus> members;
  members.push_back(elle::network::Locus("127.0.0.1", 12345));

  TemporaryDirectory tmp1;
  hole::storage::Directory storage1(tmp1.path().native());
  hole::implementations::slug::Implementation s1(
    storage1, passport, authority, members, 12345,
    boost::posix_time::seconds(1));
  s1.join();

  TemporaryDirectory tmp2;
  hole::storage::Directory storage2(tmp2.path().native());
  hole::implementations::slug::Implementation s2(
    storage1, passport, authority, members, 12345,
    boost::posix_time::seconds(1));
  s2.join();

  nucleus::proton::Network network("namespace");

  elle::cryptography::KeyPair user_keys(
    elle::cryptography::KeyPair::generate());

  nucleus::neutron::Object block(network, user_keys.K,
                                 nucleus::neutron::Genre::file);
  block.Update(nucleus::neutron::Author(),
               nucleus::proton::Address::null,
               42,
               nucleus::proton::Address::null,
               nucleus::neutron::Token::Null);
  block.Seal(user_keys.k, nucleus::neutron::Access::Null);

  auto address = block.bind();
  s1.push(address, block);

  auto retreived = elle::cast<nucleus::neutron::Object>::runtime(
    s2.pull(address, nucleus::proton::Revision::Last));
  BOOST_CHECK(retreived);
  BOOST_CHECK_EQUAL(retreived->size(), 42);

  elle::concurrency::scheduler().terminate();
}


void sched_wrapper(std::function<void()> const& f)
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
  slug->add(TEST_CASE(test));

  boost::unit_test::framework::master_test_suite().add(slug);

  return true;
}

int
main(int argc, char** argv)
{
  auto res = ::boost::unit_test::unit_test_main(test_suite, argc, argv);

  return res;
}

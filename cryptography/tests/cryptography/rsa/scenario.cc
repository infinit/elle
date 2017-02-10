#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# include "scenario.hh"

/*---------.
| Scenario |
`---------*/

// The following scenario consists in creating an object and granting access
// to two groups.
//
// Over time, the object is going to evolve, along with the access to the two
// groups. In parallel, the groups also evolve with members joining/leaving,
// action represented by the group rotation mechanism.
//
// The goal of this scenario is to show that users who have been evicted from
// a group can still access objects (versions of objects) from when the user
// was a member but cannot access the newer versions, also known as lazy
// revocation.
//
// Even more, the scenario emphasizes the ability for a new member of a group
// to be able to access past versions of objects even though he was not a
// member at the time the access to the object in question was granted.
//
// IMPORTANT!
// One must take into consideration that rotating/deriving takes quite a lot
// of time depending on the size of the key and seed. As such, it is
// recommended to update the version of the groups referenced in the ACL in
// order to reduce the derivations one needs to retrieve the correct pass.
//
// Another solution is to use a specific rotation/derivation key rather than
// using the group manager's. By keeping the key small (while large enough to
// be secure e.g 1024) and the seed large, one could speed up the process.
static
void
test_scenario()
{
  // Create the keypair for the object's owner.
  elle::cryptography::rsa::KeyPair owner =
    elle::cryptography::rsa::keypair::generate(2048);

  // Create the object and store some information in it.
  std::string content0("When I'm sad, I stop being sad "
                       "and be awesome instead");
  test::Object object0(owner, content0);
  elle::cryptography::SecretKey key0 = object0.key(owner.k());

  // A user tries to access the content without the right key.
  {
    elle::cryptography::SecretKey key =
      elle::cryptography::secretkey::generate(256);

    BOOST_CHECK_THROW(object0.read(key),
                      elle::cryptography::Error);
  }

  // Create two users.
  elle::cryptography::rsa::KeyPair userX =
    elle::cryptography::rsa::keypair::generate(1024);
  elle::cryptography::rsa::KeyPair userY =
    elle::cryptography::rsa::keypair::generate(1024);

  // Create group A with user X as a member.
  elle::cryptography::rsa::KeyPair managerA =
    elle::cryptography::rsa::keypair::generate(2048);
  test::Group groupA0(managerA.K());
  elle::cryptography::rsa::PrivateKey passA0_k =
    groupA0.pass_k(managerA.k());
  elle::cryptography::rsa::Seed seedA0 = groupA0.seed(managerA.k());
  groupA0.members().add(userX.K(), passA0_k, seedA0);
  BOOST_CHECK_EQUAL(groupA0.members().size(), 1);

  // Create group B with user Y as a member
  elle::cryptography::rsa::KeyPair managerB =
    elle::cryptography::rsa::keypair::generate(2048);
  test::Group groupB0(managerB.K());
  elle::cryptography::rsa::PrivateKey passB0_k =
    groupB0.pass_k(managerB.k());
  elle::cryptography::rsa::Seed seedB0 = groupB0.seed(managerB.k());
  groupB0.members().add(userY.K(), passB0_k, seedB0);
  BOOST_CHECK_EQUAL(groupB0.members().size(), 1);

  // Distribute the key to object 0 to both groups.
  object0.acl().grant(groupA0.address(),
                      groupA0.version(),
                      groupA0.pass_K(),
                      key0);
  object0.acl().grant(groupB0.address(),
                      groupB0.version(),
                      groupB0.pass_K(),
                      key0);

  BOOST_CHECK_EQUAL(object0.acl().size(), 2);

  // Both users X and Y should have access to object 0's content. Let's
  // try with user Y.
  {
    elle::cryptography::rsa::PrivateKey _passB0_k =
      groupB0.members().pass_k(userY);
    elle::cryptography::SecretKey _key0 =
      object0.acl().key(groupB0.address(), _passB0_k);

    BOOST_CHECK_EQUAL(key0, _key0);
    BOOST_CHECK_EQUAL(object0.read(_key0), content0);
  }

  // Rotate the group A twice.
  test::Group groupA1 = groupA0.rotate(managerA.k());
  test::Group groupA2 = groupA1.rotate(managerA.k());

  BOOST_CHECK_EQUAL(groupA2.version(), 2);
  BOOST_CHECK_EQUAL(groupA2.members().size(), 1);

  elle::cryptography::rsa::PrivateKey passA2_k =
    groupA2.pass_k(managerA.k());
  elle::cryptography::rsa::Seed seedA2 = groupA2.seed(managerA.k());

  // Update the object to version 1.
  std::string content1("If you don't have ability, "
                       "you wind up playing in a rock band");
  test::Object object1 = object0.write(owner.k(), content1);
  elle::cryptography::SecretKey key1 = object1.key(owner.k());

  // Update the object's ACL to reference the latest version of the
  // referenced groups.
  object1.acl().update(key1,
                       groupA2.address(),
                       groupA2.version(),
                       groupA2.pass_K());

  // While the existing groups have access to the new version of
  // the object, the following revokes group B's access.
  // revoke the access to group B.
  object1.acl().revoke(groupB0.address());

  BOOST_CHECK_EQUAL(object1.acl().size(), 1);

  // User Y, belonging to group B, cannot access object 1 though he can
  // still access object 0.
  {
    elle::cryptography::rsa::PrivateKey _passB0_k =
      groupB0.members().pass_k(userY);

    BOOST_CHECK_THROW(object1.acl().key(groupB0.address(), _passB0_k),
                      elle::cryptography::Error);

    elle::cryptography::SecretKey _key0 =
      object0.acl().key(groupB0.address(), _passB0_k);

    BOOST_CHECK_EQUAL(object0.read(_key0), content0);
  }

  // User X however can access object 1.
  {
    elle::cryptography::rsa::PrivateKey _passA2_k =
      groupA2.members().pass_k(userX);
    elle::cryptography::SecretKey _key1 =
      object1.acl().key(groupA2.address(), _passA2_k);

    BOOST_CHECK_EQUAL(key1, _key1);
    BOOST_CHECK_EQUAL(object1.read(_key1), content1);
  }

  // Update the object to version 2.
  std::string content2("Have no fear of perfection, you'll "
                       "never reach it");
  test::Object object2 = object1.write(owner.k(), content2);
  elle::cryptography::SecretKey key2 = object2.key(owner.k());

  // Update the object's ACL to reference the latest version of the
  // referenced groups.
  object2.acl().update(key2,
                       groupA2.address(),
                       groupA2.version(),
                       groupA2.pass_K());

  // Rotate the group a little bit more.
  test::Group groupA3 = groupA2.rotate(managerA.k());
  test::Group groupA4 = groupA3.rotate(managerA.k());
  test::Group groupA5 = groupA4.rotate(managerA.k());

  elle::cryptography::rsa::PrivateKey passA5_k =
    groupA5.pass_k(managerA.k());
  elle::cryptography::rsa::Seed seedA5 = groupA5.seed(managerA.k());

  // Add user Z to group A.
  elle::cryptography::rsa::KeyPair userZ =
    elle::cryptography::rsa::keypair::generate(1024);
  groupA5.members().add(userZ.K(), passA5_k, seedA5);
  BOOST_CHECK_EQUAL(groupA5.members().size(), 2);

  // User Z must theoretically be allowed to access object 2 since he just
  // got granted access to group A.
  //
  // He however was not recorded in the previous versions of its group A's
  // ACL entries. Let's verify this by trying to retrieve pass_k from the
  // members of group in the previous versions.
  {
    groupA5.members().retrieve(userZ.K());

    BOOST_CHECK_THROW(groupA0.members().retrieve(userZ.K()),
                      elle::cryptography::Error);
    BOOST_CHECK_THROW(groupA1.members().retrieve(userZ.K()),
                      elle::cryptography::Error);
    BOOST_CHECK_THROW(groupA2.members().retrieve(userZ.K()),
                      elle::cryptography::Error);
    BOOST_CHECK_THROW(groupA3.members().retrieve(userZ.K()),
                      elle::cryptography::Error);
    BOOST_CHECK_THROW(groupA4.members().retrieve(userZ.K()),
                      elle::cryptography::Error);
  }

  // Even though user Z does not have direct access to the
  // past versions of the object, he can unrotate past passes
  // 'pass_k' from the latest version he has been given access
  // to.
  //
  // In the following, user Z tries to access to the content
  // of object 1.
  {
    test::Entry* entry = object1.acl().retrieve(groupA5.address());

    // The group A is referenced in its version 2 in the object 1's ACL.
    BOOST_CHECK_EQUAL(entry->version(), 2);

    uint32_t derivations = groupA5.version() - entry->version();
    BOOST_CHECK_EQUAL(derivations, 3);

    elle::cryptography::rsa::Seed* seed =
      new elle::cryptography::rsa::Seed(groupA5.members().seed(userZ));

    for (uint32_t i = 0; i < derivations; i++)
    {
      elle::cryptography::rsa::Seed* _seed = seed;

      seed =
        new elle::cryptography::rsa::Seed(
          groupA5.manager_K().unrotate(*_seed));

      delete _seed;
    }

    BOOST_CHECK_EQUAL(*seed, seedA2);

    elle::cryptography::rsa::PrivateKey pass_k(*seed);
    BOOST_CHECK_EQUAL(pass_k, passA2_k);

    delete seed;

    elle::cryptography::SecretKey key =
      object1.acl().key(groupA5.address(), pass_k);
    BOOST_CHECK_EQUAL(key, key1);

    BOOST_CHECK_EQUAL(object1.read(key), content1);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/scenario");

  suite->add(BOOST_TEST_CASE(test_scenario));

  boost::unit_test::framework::master_test_suite().add(suite);
}

#endif

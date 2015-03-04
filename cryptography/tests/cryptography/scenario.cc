#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# include "scenario.hh"

/*---------.
| Scenario |
`---------*/

// The following scenario consists in creating an object and granting access
// to it to two groups.
//
// Over time, the objec is going to evolve, along with the access to the two
// groups. In parallel, the groups also evolve with members joining/leaving,
// action represented by the group rotation mechanism.
//
// The goal of this scenario is to show that users who have been evicted from
// a group can still access objects (versions of objects) from when the user
// was a member but cannot access the newer versions.
//
// Even more, the scenario emphasizes the ability for a new member of a group
// to be able to access past version of objects even though he was not a member
// at the time the access to the object in question was granted.
//
// IMPORTANT!
// One must take into consideration that rotating/deriving takes quite a lot
// of time depending on the size of the key and seed. As such, it is recommended
// to update the version of the groups referenced in the ACL in order to reduce
// the derivations one needs to retrieve the correct pass.
//
// Another solution is to use a specific rotation/derivation key rather than
// using the group manager's. By keeping the key small (while large enough to
// be secure e.g 1024) and the seed large, one could speed up the process.
static
void
_test_scenario(infinit::cryptography::Cryptosystem const cryptosystem)
{
  // Create the keypair for the object's owner.
  infinit::cryptography::KeyPair owner =
    infinit::cryptography::KeyPair::generate(cryptosystem, 2048);

  // Create the object and store some information in it.
  elle::String content0("When I'm sad, I stop being sad "
                        "and be awesome instead");
  Object object0(owner, content0);
  infinit::cryptography::SecretKey key0 = object0.key(owner.k());

  // A user tries to access the content without the right key.
  {
    infinit::cryptography::SecretKey key =
      infinit::cryptography::SecretKey::generate(
        infinit::cryptography::cipher::Algorithm::aes256,
        256);

    BOOST_CHECK_THROW(object0.read(key),
                      infinit::cryptography::Exception);
  }

  // Create two users.
  infinit::cryptography::KeyPair userX =
    infinit::cryptography::KeyPair::generate(cryptosystem, 1024);
  infinit::cryptography::KeyPair userY =
    infinit::cryptography::KeyPair::generate(cryptosystem, 1024);

  // Create group A with user X as a member.
  infinit::cryptography::KeyPair managerA =
    infinit::cryptography::KeyPair::generate(cryptosystem, 2048);
  Group groupA0(managerA.K());
  infinit::cryptography::PrivateKey passA0_k = groupA0.pass_k(managerA.k());
  infinit::cryptography::Seed seedA0 = groupA0.seed(managerA.k());
  groupA0.members().add(userX.K(), passA0_k, seedA0);
  BOOST_CHECK_EQUAL(groupA0.members().size(), 1);

  // Create group B with user Y as a member
  infinit::cryptography::KeyPair managerB =
    infinit::cryptography::KeyPair::generate(cryptosystem, 2048);
  Group groupB0(managerB.K());
  infinit::cryptography::PrivateKey passB0_k = groupB0.pass_k(managerB.k());
  infinit::cryptography::Seed seedB0 = groupB0.seed(managerB.k());
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
    infinit::cryptography::PrivateKey _passB0_k =
      groupB0.members().pass_k(userY);
    infinit::cryptography::SecretKey _key0 =
      object0.acl().key(groupB0.address(), _passB0_k);

    BOOST_CHECK_EQUAL(key0, _key0);
    BOOST_CHECK_EQUAL(object0.read(_key0), content0);
  }

  // Rotate the group A twice.
  Group groupA1 = groupA0.rotate(managerA.k());
  Group groupA2 = groupA1.rotate(managerA.k());

  BOOST_CHECK_EQUAL(groupA2.version(), 2);
  BOOST_CHECK_EQUAL(groupA2.members().size(), 1);

  infinit::cryptography::PrivateKey passA2_k = groupA2.pass_k(managerA.k());
  infinit::cryptography::Seed seedA2 = groupA2.seed(managerA.k());

  // Update the object to version 1.
  elle::String content1("If you don't have ability, "
                        "you wind up playing in a rock band");
  Object object1 = object0.write(owner.k(), content1);
  infinit::cryptography::SecretKey key1 = object1.key(owner.k());

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
    infinit::cryptography::PrivateKey _passB0_k =
      groupB0.members().pass_k(userY);

    BOOST_CHECK_THROW(object1.acl().key(groupB0.address(), _passB0_k),
                      infinit::cryptography::Exception);

    infinit::cryptography::SecretKey _key0 =
      object0.acl().key(groupB0.address(), _passB0_k);

    BOOST_CHECK_EQUAL(object0.read(_key0), content0);
  }

  // User X however can access object 1.
  {
    infinit::cryptography::PrivateKey _passA2_k =
      groupA2.members().pass_k(userX);
    infinit::cryptography::SecretKey _key1 =
      object1.acl().key(groupA2.address(), _passA2_k);

    BOOST_CHECK_EQUAL(key1, _key1);
    BOOST_CHECK_EQUAL(object1.read(_key1), content1);
  }

  // Update the object to version 2.
  elle::String content2("Have no fear of perfection, you'll "
                        "never reach it");
  Object object2 = object1.write(owner.k(), content2);
  infinit::cryptography::SecretKey key2 = object2.key(owner.k());

  // Update the object's ACL to reference the latest version of the
  // referenced groups.
  object2.acl().update(key2,
                       groupA2.address(),
                       groupA2.version(),
                       groupA2.pass_K());

  // Rotate the group a little bit more.
  Group groupA3 = groupA2.rotate(managerA.k());
  Group groupA4 = groupA3.rotate(managerA.k());
  Group groupA5 = groupA4.rotate(managerA.k());

  infinit::cryptography::PrivateKey passA5_k = groupA5.pass_k(managerA.k());
  infinit::cryptography::Seed seedA5 = groupA5.seed(managerA.k());

  // Add user Z to group A.
  infinit::cryptography::KeyPair userZ =
    infinit::cryptography::KeyPair::generate(cryptosystem, 1024);
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
                      infinit::cryptography::Exception);
    BOOST_CHECK_THROW(groupA1.members().retrieve(userZ.K()),
                      infinit::cryptography::Exception);
    BOOST_CHECK_THROW(groupA2.members().retrieve(userZ.K()),
                      infinit::cryptography::Exception);
    BOOST_CHECK_THROW(groupA3.members().retrieve(userZ.K()),
                      infinit::cryptography::Exception);
    BOOST_CHECK_THROW(groupA4.members().retrieve(userZ.K()),
                      infinit::cryptography::Exception);
  }

  // Even though user Z does not have direct access to the
  // past versions of the object, he can derive past passes
  // 'pass_k' from the latest version he has been given access
  // to.
  //
  // In the following, user Z tries to access to the content
  // of object 1.
  {
    Entry* entry = object1.acl().retrieve(groupA5.address());

    // The group A is referenced in its version 2 in the object 1's ACL.
    BOOST_CHECK_EQUAL(entry->version(), 2);

    elle::Natural32 derivations = groupA5.version() - entry->version();
    BOOST_CHECK_EQUAL(derivations, 3);

    infinit::cryptography::Seed* seed =
      new infinit::cryptography::Seed(groupA5.members().seed(userZ));

    for (elle::Natural32 i = 0; i < derivations; i++)
    {
      infinit::cryptography::Seed* _seed = seed;

      seed =
        new infinit::cryptography::Seed(
          groupA5.manager_K().derive(*_seed));

      delete _seed;
    }

    BOOST_CHECK_EQUAL(*seed, seedA2);

    infinit::cryptography::PrivateKey pass_k(*seed);
    BOOST_CHECK_EQUAL(pass_k, passA2_k);

    infinit::cryptography::SecretKey key =
      object1.acl().key(groupA5.address(), pass_k);
    BOOST_CHECK_EQUAL(key, key1);

    BOOST_CHECK_EQUAL(object1.read(key), content1);
  }
}

static
void
test_scenario()
{
  // RSA
  _test_scenario(infinit::cryptography::Cryptosystem::rsa);
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("Seed");

  suite->add(BOOST_TEST_CASE(test_scenario));

  boost::unit_test::framework::master_test_suite().add(suite);
}

#endif

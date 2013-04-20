#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/Seed.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

//#include <comet/Comet.hh> // XXX

/*----------.
| Represent |
`----------*/

void
test_represent()
{
  // These generate base64-based representations which can be used in
  // other tests.

  // XXX
}

/*---------.
| Generate |
`---------*/

void
test_generate()
{
  // XXX
}

/*----------.
| Construct |
`----------*/

void
test_construct()
{
  // XXX
}

/*--------.
| Operate |
`--------*/

void
test_operate()
{
  // XXX
}

/*----------.
| Serialize |
`----------*/

void
test_serialize()
{
  // XXX
}

/*-----.
| Main |
`-----*/

bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("Seed");

  // To uncomment if one wants to update the representations.
  //suite->add(BOOST_TEST_CASE(test_represent));

  // XXX
  {
    infinit::cryptography::KeyPair pair =
      infinit::cryptography::KeyPair::generate(
        infinit::cryptography::Cryptosystem::rsa, 1024);

    infinit::cryptography::Seed seed0 =
      infinit::cryptography::Seed::generate(pair);

    elle::printf("pair: %s\n", pair);
    elle::printf("seed0: %s\n", seed0);

    // XXX
    /* XXX
    {
      ::EVP_PKEY* key = nullptr;

      if ((key = ::EVP_PKEY_new()) == nullptr)
        throw Exception("unable to allocate the EVP_PKEY: %s",
                        ::ERR_error_string(ERR_get_error(), nullptr));

      ::RSA* rsa;

      if ((rsa = ::RSA_new()) == nullptr)
        throw Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      comet::RSA_rotate(rsa,
                        1024,
                        seed.buffer().contents(), seed.buffer().size());

      if (::EVP_PKEY_assign_RSA(key, rsa) <= 0)
        throw Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));
    }
    */

    /* XXX
    infinit::cryptography::Seed seed1 = pair.k().rotate(seed0);
    elle::printf("seed1: %s\n", seed1);

    infinit::cryptography::Seed seed2 = pair.k().rotate(seed1);
    elle::printf("seed2: %s\n", seed2);

    // XXX infinit::cryptography::KeyPair pair2(seed2);

    infinit::cryptography::Seed seed3 = pair.k().rotate(seed2);
    elle::printf("seed3: %s\n", seed3);
    */

    //infinit::cryptography::Seed _seed2 = pair.K().derive(seed3);
    //elle::printf("_seed2: %s\n", _seed2);

    // XXX verifier que lorsque l'on derive, l'exposant est toujours different et pas un truc genre 5 ou 17 sinon la derivation n'est pas privee.

    /* XXX
       Rotate from owner:
       - il recupere le seed_x courant en le dechiffrant du bloc Group.
       - on rotate le seed avec la clef privee du owner:
         k.rotate(seed_x) -> seed_x+1
       - on cree une nouvelle keypair avec le nouveau seed:
         KeyPair kp_x+1(seed_x+1) -> (Kpass_x+1, kpass_x+1)
         cet appel va utiliser comet::RSA_rotate(bits, seed) -> rsa
       - on chiffre le seed_x+1 pour que le owner puisse y acceder et
         on le met dans le nouveau Groupe
       - on inclue egalement kpass_x+1 qui est la partie "publique" du pass.
       - on chiffre et distribue Kpass_x+1 a tous les membres
       NOTE: puisque le owner devrait egalement avoir acces a Kpass_x+1 et
             puisque stocker seed prend de la place dans les metadata du
             Group, il serait plus judicieux de creer un record pour le owner
             directement dans Members. notamment avec Porcupine ca ne signifie
             pas que les donnees seront dans un autre bloc.

       Derive from member:
       - il dechiffre Kpass_x ainsi que seed_x
       - il derive seed_x: K.derive(seed_x) -> seed_x-1
         seed_x-1 contient N_x-1
       NOTE: pour ce dernier point, Seed doit etre une representation generique
             avec rsa::Seed la vraie seed. On devrait par ailleurs effectuer des
             verifications sur le fait que les clefs/seed sont compatibles.
       - ensuite on construit Kpass_x-1 a partir du seed:
         PublicKey Kpass_x-1(seed_x-1)
    */

    /* XXX
    infinit::cryptography::PublicKey _K2(_seed2);
    infinit::cryptography::KeyPair _pair2(_seed2); // XXX + k2

    BOOST_CHECK_EQUAL(pair2, _pair2);

    infinit::cryptography::Seed _seed1 = pair.K().derive(_seed2);
    _seed1.buffer().dump();

    infinit::cryptography::Seed _seed0 = pair.K().derive(_seed0);
    _seed0.buffer().dump();

    BOOST_CHECK_EQUAL(seed0, _seed0);
    */
  }
  // XXX

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}

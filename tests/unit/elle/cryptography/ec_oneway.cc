#include <iostream>

#include <elle/cryptography/oneway.hh>
#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Digest.hh>

int main()
{
  elle::Buffer buffer((elle::Byte const*)"suce.mon.cul", 12);
  elle::cryptography::Plain plain(buffer);

  elle::cryptography::Digest digest_md5(
    elle::cryptography::oneway::hash(
      plain,
      elle::cryptography::oneway::Algorithm::md5));

  // a81cf1c99a9833e027c9d5a33e9b85cd

  elle::cryptography::Digest digest_sha(
    elle::cryptography::oneway::hash(
      plain,
      elle::cryptography::oneway::Algorithm::sha));

  // 51f973db2070983d562bd5d744eea4e547de09fe

  elle::cryptography::Digest digest_sha1(
    elle::cryptography::oneway::hash(
      plain,
      elle::cryptography::oneway::Algorithm::sha1));

  // 1e408a2e1dace99ab9b3035f371417e684c088fc

  elle::cryptography::Digest digest_sha224(
    elle::cryptography::oneway::hash(
      plain,
      elle::cryptography::oneway::Algorithm::sha224));

  // 909300b8bb726fbe15199967a2648855fb9bedf608c0446b366935b0

  elle::cryptography::Digest digest_sha256(
    elle::cryptography::oneway::hash(
      plain,
      elle::cryptography::oneway::Algorithm::sha256));

  // b8bb6458a32b31734314985cfe300cec4d4a592a45c9859695bf2a2bdc9c0446

  elle::cryptography::Digest digest_sha384(
    elle::cryptography::oneway::hash(
      plain,
      elle::cryptography::oneway::Algorithm::sha384));

  // a1ded70921fbe3afa87c856596de58a1a4e0cb43369c5df1ffe9a70744360f1574380c302077a5c92113a8f3d2b3cb68

  elle::cryptography::Digest digest_sha512(
    elle::cryptography::oneway::hash(
      plain,
      elle::cryptography::oneway::Algorithm::sha512));

  // 3d22102fe74af8377f2fe9e48fa5448177b2621da77e9496edb667741c437c7a61f9f5436989d11a93830adc530235bdc34efb258fd05ca32688f5c809ea199f

  std::cout << "tests done.\n";

  return 0;
}

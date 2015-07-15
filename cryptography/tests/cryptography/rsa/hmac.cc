#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/hmac.hh>
#include <cryptography/random.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>

#include <elle/test.hh>
#include <elle/serialization/json.hh>

static std::string const _message(
  "- Back off Susan Boyle!");

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_represent_n(infinit::cryptography::rsa::PrivateKey const& k)
{
  // N)
  {
    elle::Buffer digest = infinit::cryptography::hmac::sign(_message, k, O);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("digest", digest);
    }

    elle::printf("[representation %s] %s\n", N, stream.str());
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate hexadecimal-based representations which can be used in
  // other tests.

  // 0)
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(2048);

  std::stringstream stream;
  {
    typename elle::serialization::json::SerializerOut output(stream);
    infinit::cryptography::rsa::PublicKey K = keypair.K();
    K.serialize(output);
  }
  elle::printf("[representation 0] %s\n", stream.str());

  // MD5.
  test_represent_n<1, infinit::cryptography::Oneway::md5>(keypair.k());
  // SHA.
  test_represent_n<2, infinit::cryptography::Oneway::sha>(keypair.k());
  // SHA-1.
  test_represent_n<3, infinit::cryptography::Oneway::sha1>(keypair.k());
  // SHA-224.
  test_represent_n<4, infinit::cryptography::Oneway::sha224>(keypair.k());
  // SHA-256.
  test_represent_n<5, infinit::cryptography::Oneway::sha256>(keypair.k());
  // SHA-384.
  test_represent_n<6, infinit::cryptography::Oneway::sha384>(keypair.k());
  // SHA-512.
  test_represent_n<7, infinit::cryptography::Oneway::sha512>(keypair.k());
}

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(2048);

  // HMAC sign/verify a random content.
  {
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(49);

    elle::Buffer digest =
      infinit::cryptography::hmac::sign(
        buffer,
        keypair.k(),
        infinit::cryptography::Oneway::sha1);

    BOOST_CHECK_EQUAL(infinit::cryptography::hmac::verify(
                        digest,
                        buffer,
                        keypair.K(),
                        infinit::cryptography::Oneway::sha1),
                      true);
  }

  // Try to verify an invalid HMAC.
  {
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(16);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::hmac::verify(
        buffer,
        _message,
        keypair.K(),
        infinit::cryptography::Oneway::md5),
      false);
  }
}

/*----------.
| Serialize |
`----------*/

template <infinit::cryptography::Oneway O>
void
test_serialize_x(infinit::cryptography::rsa::PublicKey const& K,
                 elle::String const& R)
{
  std::stringstream stream(R);
  typename elle::serialization::json::SerializerIn input(stream);
  elle::Buffer digest;
  input.serialize("digest", digest);

  BOOST_CHECK_EQUAL(
    infinit::cryptography::hmac::verify(
      digest,
      _message,
      K,
      O),
    true);
}

static
void
test_serialize()
{
  // Extract the key pair from [representation 0].
  elle::String representation0(R"JSON({"private key":{"digest algorithm":4,"encryption padding":3,"rsa":"MIIEpAIBAAKCAQEAwYfl1tHiNwZ1Yk3lQ01KPJls62H6w3OoA68Cv2JpWTFRIcjqDH75No2rjs5yIQnZgQ02mlJ9b0Y39ofq7G49bNcqp5juNQiKgZ5/tK+wZKmNttDpETpj/nJmIbrieMNTPws9k2V8FGR+nS2exG6N//CXbkph67UOep5FqpJzboZPI4aO8tVyurXFBaBainpuvGwM7O5jj4PO8D3cjMQOZrXgAZPoiGX0daI+9IfTnocOTBMvplNlEJMLj5RkOSzFK/yGQozwlg2XIDiYV+r944qTrbdXpltqlkDKtdZRE9VTiY4Duj0qt/Jv42rT8TbFZ9ebwrh6JwnRXX5JCDBDxwIDAQABAoIBABBc0hTwl+S5GhGermSbU0x+WFQqa4486BavpFbUFj4hSk/F8evujFmNbtTbJZEfiRE9McGoecx/rlmcskMdKvpZ0tRqvLElMLi20utO82X/Y+jyDtKTEkNAHtx5NY6qTQ6Gf8VHjk8XFBFr+wF2q3DZQaRki5TnMPszCIbcbQAKGxQ1Fn0FhnIRT4hjHXVlA3Oyy7Sz5JiLmQj4hBLyDeC2eojDOi5F9/HGiI3xXdS5TGBMY2SJdvFUKocEv5V4LcZpmEgOppgIrnPCWiI9je9sAv/9ZiS41c25xQjV/e28qdOdurQUSHnUdPvjLutZPGMN9Fu1P75zKjdNOGusWjECgYEA/JYMNo3cDXYrs7rFoOdN+Gf0fJE75JhJuchxvWDw4S6OovGbJWisYvvvaNgMW4xCK1KNcGbiw36gULP+lXPuyFS6s5RK44jkMVOyZFspufIfGI3DD94csB+qGE9eaPf4mUfz6NKsMttGhtCvM65i8uc03nFP1LIhLkzWg627aAkCgYEAxCWEkd0M0xs/MbF7slw7tzWjtqLF4viC9gkv0lFJvJeKvoGu+1PVL7nNtctzBlH5b4lzGJqlvXwutJyxXKTR6YBhzuiFSiTNzgIUja6wiFJaItWk/kVP7+RV+vRCF+Srunlu8EUT7Cr8Qz3ggN2vqLkrhZGleOL2vmT/kNPiIU8CgYEA6gBsgiH8anA+3nw62O1RG7/bDYhTT10SeE5NqR/lKQjD1gs1K3wEsJMsYho9oJaZOp95L1fx3yExjnKieXz1C50LdpD8rD1HArfCfhzC1GSOEGZM/ydLtpMvNuRY17BbHE4/lkOOinMArKcqmRuGyfgOz8F0Lt1zxfwjg8xw4VkCgYEAjCMd9zTOX9l8qd1bOhBJndjV8buT1FinAG+J2iK3ouVGfFudD/VSP/no7fTAZoiUxMqYD8EVny6lE+dLlJwxWTmfJ7va1yE6ZO/5jUx81bi4q2CmtAADX6psf5KZ0gZPZzgxObqwxC/l8Yi46Hj6cnpxjMmFRrwpe9ge+3NWgWMCgYAJlOQmdZGMijemfSO29jDHpUQIsKwJiRBFoS3+uz0RoQ3cDW0GKol0IwoRHQfLNzeWxt5fP6ftH0uE5RGum9jC1ah4w5Oc/yiu6s1m7X5zkvsHW2lULutktuo5mxj+C+aN8TxKOlqQv4hmdM51/DqzhJcxVfTzdPBVFuzXb229bA==","signature padding":5,"version":{"major":0,"minor":0,"subminor":0}},"public key":{"digest algorithm":4,"encryption padding":3,"envelope cipher":11,"envelope mode":1,"rsa":"MIIBCgKCAQEAwYfl1tHiNwZ1Yk3lQ01KPJls62H6w3OoA68Cv2JpWTFRIcjqDH75No2rjs5yIQnZgQ02mlJ9b0Y39ofq7G49bNcqp5juNQiKgZ5/tK+wZKmNttDpETpj/nJmIbrieMNTPws9k2V8FGR+nS2exG6N//CXbkph67UOep5FqpJzboZPI4aO8tVyurXFBaBainpuvGwM7O5jj4PO8D3cjMQOZrXgAZPoiGX0daI+9IfTnocOTBMvplNlEJMLj5RkOSzFK/yGQozwlg2XIDiYV+r944qTrbdXpltqlkDKtdZRE9VTiY4Duj0qt/Jv42rT8TbFZ9ebwrh6JwnRXX5JCDBDxwIDAQAB","signature padding":5,"version":{"major":0,"minor":0,"subminor":0}}})JSON");

  std::stringstream stream(representation0);
  typename elle::serialization::json::SerializerIn input(stream);
  infinit::cryptography::rsa::KeyPair keypair(input);

  // MD5 based on [representation 1].
  test_serialize_x<infinit::cryptography::Oneway::md5>(
    keypair.K(),
    R"JSON({"digest":"dNVxHTiiXhMdOvRcoDTkNOAYJXXQfDGhB4OsZqFxz4AXF6Qbb31Va3M/uSSgxHrOpAd1H/phNlkwDhdzam1kSOoH7ENvVzsZQRvNOJ6SG2hISNkbn4j+4rlf7yNSJNlxCDyqJaMviahJmjWZM8GPBL3BhtdGnOQkJRcOo3nPaIeY4GX9V8gcx3Pl6r8NoYVnTTxayyIXCXIrJSHC38DAbGLDHlh7wYNOzSyY63HK1XBhFnlF1KWDUlhVD3szLJFwXPHNkhEZuP2p/JeCnNgLSVTBzk2VMpke9SOlmXF2HBcqgXKhyNpiFJ82LCMsRs6DBobwNKYageVCLsKKVj+iaA=="})JSON");
  // SHA based on [representation 2].
  test_serialize_x<infinit::cryptography::Oneway::sha>(
    keypair.K(),
    R"JSON({"digest":"r0DBmCfew4go5NVXV0IF9UjNYmVZ7KiyshI+JEyWpXhlc6PNCyz3DkMs+SPV03iOFN0iIUZ3DaKkEzTr3QEZvJWgZSi7opJ4O9Q5J1ytopgPeZBjadq8k7lZ6k/CJvIs9ZvR/gNST0k9Z5xckLgaCdfb0GOj16zQKt+Lj2NYgfzrWDNK360woyz51h34+E/9bRemBMM5KT29J8u6eprwVQRKyEYKPXX1Ub5bhUoePlC5guN2uKjoDAwv+p6pSlTuCm80FVwngJdqVcsme14QLBJRv/46yCIOw/Gbs4KIsA+xX7CmH2rxB6VqBvzv+ZlILFyehc5J53ZHNIEnwxmFIw=="})JSON");
  // SHA-1 based on [representation 3].
  test_serialize_x<infinit::cryptography::Oneway::sha1>(
    keypair.K(),
    R"JSON({"digest":"NMVEiMQsSqjqQQdCsLuyQ2Me5LlfSoAVWdC2q9PpswxYFoJdfNPEwUnwGjC68YB6JrY0XQESZq4n5ID1iu6DCMDchSBxVb+uZrWmoy0wRljQtwsbUQ+XbOJgnMTRkR76KfliytBLzJELXF5vPKN9g65nQeb6+emslfcpqfh/Xey9lzOLya23HkaCVHq+4YXdTT6hQLiEZcejVHnvIawyg2/DqU5da9/od7WoqyhhEXxB/b5PpFCTX8QIbLa5/oYrRsIxrvD2sunb48GlRkLuHDOisnx4dWI7Fkr5hTCWQwWlpl5QIPgqVf7GHXb+iqaQcNuhkfQ60sQbbUTW39kktg=="})JSON");
  // SHA-224 based on [representation 4].
  test_serialize_x<infinit::cryptography::Oneway::sha224>(
    keypair.K(),
    R"JSON({"digest":"K/o4ZGpa5uEED76Ln9c4oQi/V2/DarjuoZ0nIZBy9J1FRRWNrILt5A8YHYC5A4ECHOygynfoNdLytXgw3KxZvhC2vWruckWrL2ByypfGkT/Unest/CFZw4KuR2xCwJVw7z53NwxFYolxJbkOgAUjwKNSqQEHLN4xq5xOzpBb0Il+Y8wblvdPfei7DQi5+l8AzT4M/esLCd0V+IPWjWFOqQ8k4ac96y/jMcslGz5yYSE3Z7gUvSycgdswq8t5I36aXan8UA78p2b3ZyMhcONPPQRU9EEiIaRBpxhVYiWjrAON9sWxh3+EU8neOXUVH83QtfEK2b07EmHllhU5ChHX+Q=="})JSON");
  // SHA-256 based on [representation 5].
  test_serialize_x<infinit::cryptography::Oneway::sha256>(
    keypair.K(),
    R"JSON({"digest":"tXhoH1bA5GGBzXGreSL+I6qIt8tHpMtos0av1s67CCipIBhEZdWSYGR0VaEX7bmDvP97hokR+hoPz4z20OPQpNBRLvQJZ4ci2KbgdD9WGj+lv1SBo4OCrWd6r7GCGIGEW9bJyWM5XHucfYP5pws/uKRbCL536SjmrTXy8FnbwlJyq/iouVKQBGV8XoyM7knpA/TifxqYmAbBWUNohr6S9Bx+sGW5hRr2jWaBWcxYupKTXEAoCNFPz/iDnBAp0I9Td4zlqj/0MUDp5IU2hTDY3c33AUWftwKbRNuIxyUhaPF09/9PnjeS15NHj4Nwb6VuIa7kX4/NOSVCuCKwegOBgQ=="})JSON");
  // SHA-384 based on [representation 6].
  test_serialize_x<infinit::cryptography::Oneway::sha384>(
    keypair.K(),
    R"JSON({"digest":"qDX75p0BPpZGm+snpBCcCJG1Zo+UnGVFRfTYVFTwQahpvIlW9wOHPq7LPe5LMuAi+B0EXH3gL8yDh3ezYAAeZcJE/XLVdijkCbdfBJHcXSPuKVDXZaELtphZbomxMtZh6Hp0b+Qt3l4uq58yGAmuSJiBYwh7RZbg5u26xIuqhdNvgUQhGO60V6YzmRsO2sRlN4JHUpKrwAJKR7eZmYNjwyTR/K9GXlNX+VRHWBPPlG/PGTm6TZSEPlu1UGalC/4Mqqpgy4kQtkTxnrmXBsoeXvmByQwnCwgcKEzArlPqh4tC+l8oz8Gz9RVmD+9nM7BXtN2nw+vMIC0TT92xeEd49A=="})JSON");
  // SHA-512 based on [representation 7].
  test_serialize_x<infinit::cryptography::Oneway::sha512>(
    keypair.K(),
    R"JSON({"digest":"is9tKGJsbLDNr31veOtsV6bVuEYyfLjOoyInxaq9v16H5a8rt4sXkXmJ6vrFWPaPx8IfCauaTkZMoCyo82J11te2MhnL1Wp9LgJOokQ2g5SquBwV9uCAMC89fYB23aNhwWquK7o8HHceiwaWIG76dyBPEFoSXBg8TC2a1JS8XdXaKwku84SAqmkscXautjlfyO6itrWwlKuFaZSFJbrUnyt3ggV2U24G/h1O/t07A8M+I7CucOAM5WrvBP+WW3YZUSK3uWFGsM4gAGIKstBG4rjKVphlzSJSOu9WhrVYMHNMJpLZ05/7+d8w8JDpiDhipMQRKeW41fF4c1KdL4IpJg=="})JSON");
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/hmac");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}

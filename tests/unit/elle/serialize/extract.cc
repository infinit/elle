#include <elle/serialize/extract.hh>

#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

int main()
{
  using elle::serialize::from_string;
  using elle::serialize::InputBase64Archive;
  using elle::serialize::OutputBase64Archive;

  // This string has been generated as follows:
  //
//       elle::io::Unique unique;
//       cryptography::KeyPair kp = cryptography::KeyPair::generate(1024);
//       if (kp.Save(unique) == elle::Status::Error)
//         throw elle::Exception("unable XXX");
//       std::cout << unique << std::endl;

  std::string string("AAAAAAAAgAAAALkv7mFyDOrSVRPH/aitVp0yk8eFzHtswUp6am4j4Ka8vGP4gDKHVzpEUGsr4IzFLJxsDCUGfLcWmfyqUru4Z6+ipspxZ89wiu1+/1mKSYL+B4iPLta7YAa/zVwKSyBuV+i34ez0lpJMLXea6iEb4f6DWXgLYceN0aVdndjEZpvZAAADAAAAAQABAAAAAIAAAAC5L+5hcgzq0lUTx/2orVadMpPHhcx7bMFKempuI+CmvLxj+IAyh1c6RFBrK+CMxSycbAwlBny3Fpn8qlK7uGevoqbKcWfPcIrtfv9ZikmC/geIjy7Wu2AGv81cCksgblfot+Hs9JaSTC13muohG+H+g1l4C2HHjdGlXZ3YxGab2QAAAwAAAAEAAQAAgAAAAHAdaafLQbDE6EcSlCqTztVa8ocgJR0wSZGKrXbC99Zae3/Wyd5Ne+zFwR4xMUBoe0q0S2H7HHRIFbAXegE2cmakw1c3i+9BZ/hODl2tdb6RiY59yF34XJVOpzSmJj+egjzp5HXIxHmy+sXxfYkXyy4tIXFGmJs7bj7AZvTmyy9lAABAAAAA8zcgSbmoo0lekrye6GcNsFZn+VwrWfX5fNBl+HP3eOS4uzRb+RpA0J2tpRyPSTnBTpdz9H3f35dty8zlbkOFTwAAQAAAAMLr8E82GT5kaG+aDAol2AGC2kZGoNH3tA9SpkzR3UOpyfI6yRnsyc4RbnRsIfeWVa7v88qJb0wK71WK5ervUlcAAEAAAAC5G1Coq/QMaq/e8E3lRWGmvuydXafk7Q+4Dgg9400LJtDuMfh6C6Ae5pMdYjIvohRc02o+P3se+mpnPQe1NxCDAABAAAAAu6WNQ4ROCv14eK+xFXfXGaVR4kOOAHRY2EN1qxCB1Oi4a8Ph415L7GBlmBidHo+QTwfbB0kFZwxD2XMM4f2gowAAQAAAADQPoWzQB3ic+G63B/YYlrkn7yEJxYgm7Uk7mMiZBmB8zq1q01JWaSDNORmTjE3B3Y7fdyDnEjW7uHEeThrUXPQ=");

  auto extractor = from_string<InputBase64Archive>(string);
  cryptography::KeyPair pair{extractor};

  std::cout << "tests done." << std::endl;
  return 0;
}

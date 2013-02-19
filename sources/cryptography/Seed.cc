#include <cryptography/Seed.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/random.hh>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Seed::Seed(KeyPair const& pair):
      _buffer(random::generate<elle::Buffer>(pair.K().length() /
                                             sizeof (elle::Byte)))
    {
    }

    /*--------.
    | Methods |
    `--------*/

    Seed
    Seed::rotate(PrivateKey const& k) const
    {
//       Code              code;

//       // XXX si ca se trouve juste en passant Plain(this->region) ca simplifierait
//       // XXX puisque ca eviterait de serializer le tout donc ca ne grossierait pas
//       // XXX a cause de l'overhead de serialization.

//       // encrypt the seed object with the given private key.
//       if (k.Encrypt(*this, code) == elle::Status::Error)
//         throw Exception("unable to 'encrypt' the seed");

//       // detach the memory from the code.
//       if (code.region.Detach() == elle::Status::Error)
//         throw Exception("unable to detach the memory");

//       // assign the code's region to the output seed.
//       if (seed.region.Acquire(code.region.contents,
//                               code.region.size) == elle::Status::Error)
//         throw Exception("unable to acquire the region");

//       return elle::Status::Ok;
    }

    Seed
    Seed::derive(PublicKey const& K) const
    {
//       Code              code;
//       elle::standalone::Region chunk;

//       // wrap the seed's region.
//       if (chunk.Wrap(this->region.contents,
//                      this->region.size) == elle::Status::Error)
//         throw Exception("unable to wrap the region");

//       // create a code based on the chunk.
//       if (code.Create(chunk) == elle::Status::Error)
//         throw Exception("unable to create the code");

//       // decrypt the code with the given public key.
//       if (K.Decrypt(code, seed) == elle::Status::Error)
//         throw Exception("unable to 'decrypt' the seed");

//       return elle::Status::Ok;
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Seed::operator ==(Seed const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer == other._buffer);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Seed::print(std::ostream& stream) const
    {
      stream << this->_buffer;
    }
  }
}

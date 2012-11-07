#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/random.hh>

#include <cryptography/Seed.hh>

#include <iostream>

namespace infinit
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines the seed length in bits.
    ///
    const elle::Natural32             Seed::Length = 1024;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates a random seed.
    ///
    elle::Status              Seed::Generate()
    {
      return (this->Generate(Seed::Length));
    }

    ///
    /// this method generates a seed based on the given length.
    ///
    elle::Status              Seed::Generate(const elle::Natural32          length)
    {
      elle::Natural32         size;

      // compute the size in bytes.
      size = length / 8;

      // randomize the region.
      // XXX[change the attribute to a buffer]
      elle::Buffer buffer(random::generate<elle::Buffer>(size));
      if (this->region.Duplicate(buffer.contents(), buffer.size()) == elle::Status::Error)
        escape("XXX");

      return elle::Status::Ok;
    }

    ///
    /// this method rotates the seed.
    ///
    /// note that this operation should be performed by the seed initiator
    /// only.
    ///
    elle::Status              Seed::Rotate(const PrivateKey&          k,
                                     Seed&                      seed) const
    {
      Code              code;

      // encrypt the seed object with the given private key.
      if (k.Encrypt(*this, code) == elle::Status::Error)
        escape("unable to 'encrypt' the seed");

      // detach the memory from the code.
      if (code.region.Detach() == elle::Status::Error)
        escape("unable to detach the memory");

      // assign the code's region to the output seed.
      if (seed.region.Acquire(code.region.contents,
                              code.region.size) == elle::Status::Error)
        escape("unable to acquire the region");

      return elle::Status::Ok;
    }

    ///
    /// this method derives the seed with the initiator's public key.
    ///
    elle::Status              Seed::Derive(const PublicKey&           K,
                                     Seed&                      seed) const
    {
      Code              code;
      elle::standalone::Region chunk;

      // wrap the seed's region.
      if (chunk.Wrap(this->region.contents,
                     this->region.size) == elle::Status::Error)
        escape("unable to wrap the region");

      // create a code based on the chunk.
      if (code.Create(chunk) == elle::Status::Error)
        escape("unable to create the code");

      // decrypt the code with the given public key.
      if (K.Decrypt(code, seed) == elle::Status::Error)
        escape("unable to 'decrypt' the seed");

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    elle::Boolean             Seed::operator==(const Seed&            element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->region == element.region);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Seed, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the seed internals.
    ///
    elle::Status              Seed::Dump(const elle::Natural32              margin) const
    {
      elle::String            alignment(margin, ' ');

      std::cout << alignment << "[Seed]" << std::endl;

      // dump the region.
      if (this->region.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the region");

      return elle::Status::Ok;
    }

  }
}

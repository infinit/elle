
#include <iostream>

#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/Seed.hh>
#include <elle/cryptography/Random.hh>
#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines the seed length in bits.
    ///
    const Natural32             Seed::Length = 1024;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates a random seed.
    ///
    Status              Seed::Generate()
    {
      return (this->Generate(Seed::Length));
    }

    ///
    /// this method generates a seed based on the given length.
    ///
    Status              Seed::Generate(const Natural32          length)
    {
      Natural32         size;

      // compute the size in bytes.
      size = length / 8;

      // randomize the region.
      if (Random::Generate(this->region, size) == Status::Error)
        escape("unable to generate the region");

      return Status::Ok;
    }

    ///
    /// this method rotates the seed.
    ///
    /// note that this operation should be performed by the seed initiator
    /// only.
    ///
    Status              Seed::Rotate(const PrivateKey&          k,
                                     Seed&                      seed) const
    {
      Code              code;

      // encrypt the seed object with the given private key.
      if (k.Encrypt(*this, code) == Status::Error)
        escape("unable to 'encrypt' the seed");

      // detach the memory from the code.
      if (code.region.Detach() == Status::Error)
        escape("unable to detach the memory");

      // assign the code's region to the output seed.
      if (seed.region.Acquire(code.region.contents,
                              code.region.size) == Status::Error)
        escape("unable to acquire the region");

      return Status::Ok;
    }

    ///
    /// this method derives the seed with the initiator's public key.
    ///
    Status              Seed::Derive(const PublicKey&           K,
                                     Seed&                      seed) const
    {
      Code              code;
      Region            chunk;

      // wrap the seed's region.
      if (chunk.Wrap(this->region.contents,
                     this->region.size) == Status::Error)
        escape("unable to wrap the region");

      // create a code based on the chunk.
      if (code.Create(chunk) == Status::Error)
        escape("unable to create the code");

      // decrypt the code with the given public key.
      if (K.Decrypt(code, seed) == Status::Error)
        escape("unable to 'decrypt' the seed");

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean             Seed::operator==(const Seed&            element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the internal region.
      if (this->region != element.region)
        return false;

      return true;
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
    Status              Seed::Dump(const Natural32              margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Seed]" << std::endl;

      // dump the region.
      if (this->region.Dump(margin + 2) == Status::Error)
        escape("unable to dump the region");

      return Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a seed object.
    ///
    //Status              Seed::Serialize(Archive&                archive) const
    //{
    //  // serialize the internal region.
    //  if (archive.Serialize(this->region) == Status::Error)
    //    escape("unable to serialize the internal region");

    //  return Status::Ok;
    //}

    /////
    ///// this method extract a seed from the given archive.
    /////
    //Status              Seed::Extract(Archive&                  archive)
    //{
    //  // extract the region.
    //  if (archive.Extract(this->region) == Status::Error)
    //    escape("unable to extract the internal region");

    //  return Status::Ok;
    //}

  }
}

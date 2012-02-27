//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat mar 19 14:10:58 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Seed.hh>
#include <elle/cryptography/Random.hh>

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
      if (Random::Generate(this->region, size) == StatusError)
        escape("unable to generate the region");

      return elle::StatusOk;
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
      if (k.Encrypt(*this, code) == StatusError)
        escape("unable to 'encrypt' the seed");

      // detach the memory from the code.
      if (code.region.Detach() == StatusError)
        escape("unable to detach the memory");

      // assign the code's region to the output seed.
      if (seed.region.Acquire(code.region.contents,
                              code.region.size) == StatusError)
        escape("unable to acquire the region");

      return elle::StatusOk;
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
                     this->region.size) == StatusError)
        escape("unable to wrap the region");

      // create a code based on the chunk.
      if (code.Create(chunk) == StatusError)
        escape("unable to create the code");

      // decrypt the code with the given public key.
      if (K.Decrypt(code, seed) == StatusError)
        escape("unable to 'decrypt' the seed");

      return elle::StatusOk;
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
        return elle::StatusTrue;

      // compare the internal region.
      if (this->region != element.region)
        return elle::StatusFalse;

      return elle::StatusTrue;
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
      if (this->region.Dump(margin + 2) == StatusError)
        escape("unable to dump the region");

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a seed object.
    ///
    Status              Seed::Serialize(Archive&                archive) const
    {
      // serialize the internal region.
      if (archive.Serialize(this->region) == StatusError)
        escape("unable to serialize the internal region");

      return elle::StatusOk;
    }

    ///
    /// this method extract a seed from the given archive.
    ///
    Status              Seed::Extract(Archive&                  archive)
    {
      // extract the region.
      if (archive.Extract(this->region) == StatusError)
        escape("unable to extract the internal region");

      return elle::StatusOk;
    }

  }
}

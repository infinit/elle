#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# include <elle/cryptography/Error.hh>
# include <elle/cryptography/random.hh>
# include <elle/cryptography/bn.hh>
# include <elle/cryptography/rsa/Seed.hh>
# include <elle/cryptography/rsa/KeyPair.hh>

# include <elle/log.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      /*-------------.
      | Construction |
      `-------------*/

      Seed::Seed(elle::Buffer const& buffer,
                 uint32_t const length):
        _buffer(buffer),
        _length(length)
      {
      }

      Seed::Seed(elle::Buffer&& buffer,
                 uint32_t const length):
        _buffer(std::move(buffer)),
        _length(length)
      {
      }

      Seed::Seed(Seed const& other):
        _buffer(other._buffer.contents(), other._buffer.size()),
        _length(other._length)
      {
      }

      Seed::Seed(Seed&& other):
        _buffer(std::move(other._buffer)),
        _length(other._length)
      {
      }

      /*----------.
      | Operators |
      `----------*/

      bool
      Seed::operator ==(Seed const& other) const
      {
        if (this == &other)
          return (true);

        if (this->_length != other._length)
          return (false);

        if (this->_buffer != other._buffer)
          return (false);

        return (true);
      }

      /*----------.
      | Printable |
      `----------*/

      void
      Seed::print(std::ostream& stream) const
      {
        stream << this->_buffer;
      }

      /*--------------.
      | Serialization |
      `--------------*/

      Seed::Seed(elle::serialization::SerializerIn& serializer)
      {
        this->serialize(serializer);
      }

      void
      Seed::serialize(elle::serialization::Serializer& serializer)
      {
        serializer.serialize("buffer", this->_buffer);
        serializer.serialize("length", this->_length);
      }
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace seed
      {
        /*----------.
        | Functions |
        `----------*/

        Seed
        generate(uint32_t const length)
        {
          if ((length % 8) != 0)
            throw Error(
              elle::sprintf("the seed length %s is not a multiple of 8",
                            length));

          // Return an RSA seed constructed by generating a random buffer
          // of the given length.
          elle::Buffer buffer =
            random::generate<elle::Buffer>(length / 8);

          // Prepend a zero byte to make sure the buffer, once translated into
          // a big number, will always be smaller than any other number of the
          // same size.
          //
          // As an example, if the modulus is A87E9CD1, a buffer of the same
          // size must be generated, say D719A0BE. Unfortunately this buffer,
          // once transformed into a number, is bigger than the modulus (also
          // in its number format): D719A0BE > A87E9CD1.
          //
          // One could be tempted to generate a shorter buffer. Unfortunately
          // OpenSSL requires the data to be the exact same size as the modulus
          // for non-padded operations.
          //
          // The solution consists in keeping a buffer of the same size but
          // making sure that, once transformed into a number, it will always
          // be smaller than the modulus. Enforcing the first byte to zero is
          // a way to achieve this result.
          buffer.mutable_contents()[0] = 0;

          return (Seed(std::move(buffer), length));
        }

        Seed
        generate(KeyPair const& keypair)
        {
          return (generate(keypair.length()));
        }
      }
    }
  }
}

#endif

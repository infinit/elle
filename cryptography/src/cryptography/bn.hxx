#ifndef INFINIT_CRYPTOGRAPHY_BN_HXX
# define INFINIT_CRYPTOGRAPHY_BN_HXX

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
/*-------.
| Legacy |
`-------*/

#  include <elle/types.hh>
#  include <elle/finally.hh>
#  include <elle/serialize/Serializer.hh>

#  include <cryptography/Exception.hh>

#  include <openssl/err.h>

ELLE_SERIALIZE_SPLIT(::BIGNUM)

ELLE_SERIALIZE_SPLIT_SAVE(::BIGNUM,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  // Retrieve the size of the big number.
  int size = BN_num_bytes(&value);
  enforce(size > 0, "cannot save empty BIGNUM");

  unsigned char* buffer = new unsigned char[size];

  elle::SafeFinally free([&] { delete [] buffer; });

  // Copy the binary data into the buffer.
  ::BN_bn2bin(&value, buffer);

  // Serialize both the size and the binary data.
  archive << static_cast<elle::Natural32>(size);
  archive.SaveBinary(buffer, size);
}

// Note that the big number (i.e _value_) must have been initialized either
// through BN_new() or BN_init().
ELLE_SERIALIZE_SPLIT_LOAD(::BIGNUM,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  // Extract the big number size.
  elle::Natural32 size;

  archive >> size;
  enforce(size > 0, "cannot load empty BIGNUM");

  // Allocate a buffer.
  unsigned char* buffer = new unsigned char[size];

  elle::SafeFinally finally([&] { delete[] buffer; });

  // Extract the buffer.
  archive.LoadBinary(buffer, size);

  // Load the data binary and convert it into a big number.
  if (::BN_bin2bn(buffer, static_cast<int>(size), &value) == nullptr)
    throw infinit::cryptography::Exception(
      elle::sprintf("unable to convert the binary data into a big number: %s",
                    ::ERR_error_string(ERR_get_error(), nullptr)));
}
# endif

#endif

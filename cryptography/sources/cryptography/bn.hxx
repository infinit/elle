#ifndef INFINIT_CRYPTOGRAPHY_BN_HXX
# define INFINIT_CRYPTOGRAPHY_BN_HXX

/*-------------.
| Serializable |
`-------------*/

# include <elle/types.hh>
# include <elle/serialize/Serializer.hh>

# include <cryptography/cryptography.hh>
# include <cryptography/finally.hh>

ELLE_SERIALIZE_SPLIT(::BIGNUM)

ELLE_SERIALIZE_SPLIT_SAVE(::BIGNUM,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  // Make sure the cryptographic system is set up.
  infinit::cryptography::require();

  // Retrieve the size of the big number.
  int size = BN_num_bytes(&value);

  unsigned char* buffer = new unsigned char[size];

  ELLE_FINALLY_ACTION_DELETE(buffer);

  // Copy the binary data into the buffer.
  ::BN_bn2bin(&value, buffer);

  // Serialize both the size and the binary data.
  archive << size;
  archive.SaveBinary(buffer, size);

  ELLE_FINALLY_ABORT(buffer);

  delete [] buffer;
}

// Note that the big number (i.e _value_) must have been initialized either
// through BN_new() or BN_init().
ELLE_SERIALIZE_SPLIT_LOAD(::BIGNUM,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  // Make sure the cryptographic system is set up.
  infinit::cryptography::require();

  // Extract the big number size.
  int size;

  archive >> size;

  // Allocate a buffer.
  unsigned char* buffer = new unsigned char[size];

  ELLE_FINALLY_ACTION_DELETE(buffer);

  // Extract the buffer.
  archive.LoadBinary(buffer, size);

  // Load the data binary and convert it into a big number.
  ::BN_bin2bn(buffer, size, &value);

  ELLE_FINALLY_ABORT(buffer);

  delete [] buffer;
}

#endif

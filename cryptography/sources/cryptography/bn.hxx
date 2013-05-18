#ifndef INFINIT_CRYPTOGRAPHY_BN_HXX
# define INFINIT_CRYPTOGRAPHY_BN_HXX

/*-----------.
| Serializer |
`-----------*/

# include <elle/types.hh>
# include <elle/finally.hh>
# include <elle/serialize/Serializer.hh>

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

  ELLE_FINALLY_ACTION_DELETE_ARRAY(buffer);

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

  // Extract the big number size.
  int size;

  archive >> size;
  enforce(size > 0, "cannot load empty BIGNUM");

  // Allocate a buffer.
  unsigned char* buffer = new unsigned char[size];

  ELLE_FINALLY_ACTION_DELETE_ARRAY(buffer);

  // Extract the buffer.
  archive.LoadBinary(buffer, size);

  // Load the data binary and convert it into a big number.
  ::BN_bin2bn(buffer, size, &value);

  ELLE_FINALLY_ABORT(buffer);

  delete [] buffer;
}

#endif

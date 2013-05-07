#ifndef INFINIT_CRYPTOGRAPHY_BN_HXX
# define INFINIT_CRYPTOGRAPHY_BN_HXX

/*-----------.
| Serializer |
`-----------*/

# include <elle/Buffer.hh>
# include <elle/serialize/Serializer.hh>
# include <elle/types.hh>

ELLE_SERIALIZE_SPLIT(::BIGNUM)

ELLE_SERIALIZE_SPLIT_SAVE(::BIGNUM,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  int size = BN_num_bytes(&value);
  enforce(size > 0, "Invalid BIGNUM buffer size");

  elle::Buffer buffer{static_cast<size_t>(size)};
  ::BN_bn2bin(&value, buffer.mutable_contents());

  archive << buffer;
}

// Note that the big number (i.e _value_) must have been initialized either
// through BN_new() or BN_init().
ELLE_SERIALIZE_SPLIT_LOAD(::BIGNUM,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  elle::Buffer buffer;
  archive >> buffer;

  enforce(buffer.size() > 0, "Invalid BIGNUM buffer size");
  ::BN_bin2bn(buffer.contents(), buffer.size(), &value);
}

#endif

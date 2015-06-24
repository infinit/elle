#ifndef ELLE_SERIALIZATION_BINARY_HH
# define ELLE_SERIALIZATION_BINARY_HH

# include <elle/serialization/binary/SerializerIn.hh>
# include <elle/serialization/binary/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    class Binary
    {
    public:
      typedef binary::SerializerIn SerializerIn;
      typedef binary::SerializerOut SerializerOut;
    };
  }
}

#endif

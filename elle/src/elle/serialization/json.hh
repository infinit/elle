#ifndef ELLE_SERIALIZATION_JSON_HH
# define ELLE_SERIALIZATION_JSON_HH

# include <elle/serialization/json/SerializerIn.hh>
# include <elle/serialization/json/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    class Json
    {
    public:
      typedef json::SerializerIn SerializerIn;
      typedef json::SerializerOut SerializerOut;
    };
  }
}

#endif

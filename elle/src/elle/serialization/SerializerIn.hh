#ifndef ELLE_SERIALIZATION_SERIALIZERIN_HH
# define ELLE_SERIALIZATION_SERIALIZERIN_HH

# include <iosfwd>

# include <elle/attribute.hh>
# include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    class SerializerIn:
      public Serializer
    {
    public:
      SerializerIn(std::istream& input);

    protected:
      ELLE_ATTRIBUTE(std::istream&, input);
    };
  }
}

#endif

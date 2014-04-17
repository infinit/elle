#ifndef ELLE_SERIALIZATION_SERIALIZEROUT_HH
# define ELLE_SERIALIZATION_SERIALIZEROUT_HH

# include <iosfwd>

# include <elle/attribute.hh>
# include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    class SerializerOut:
      public Serializer
    {
    public:
      SerializerOut(std::ostream& output);

    protected:
      ELLE_ATTRIBUTE(std::ostream&, output);
    };
  }
}


#endif

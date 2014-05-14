#ifndef ELLE_SERIALIZATION_SERIALIZER_OUT_HH
# define ELLE_SERIALIZATION_SERIALIZER_OUT_HH

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
      std::ostream&
      output();
      ELLE_ATTRIBUTE(std::ostream&, output);
    };
  }
}

# include <elle/serialization/Serializer.hxx>

#endif

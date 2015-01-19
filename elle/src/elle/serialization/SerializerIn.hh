#ifndef ELLE_SERIALIZATION_SERIALIZER_IN_HH
# define ELLE_SERIALIZATION_SERIALIZER_IN_HH

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
    /*------.
    | Types |
    `------*/
    public:
      typedef SerializerIn Self;
      typedef Serializer Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      SerializerIn(std::istream& input);
      SerializerIn(std::istream& input, elle::Version version);

    /*--------.
    | Details |
    `--------*/
    protected:
      ELLE_ATTRIBUTE(std::istream&, input);
    };
  }
}

# include <elle/serialization/Serializer.hxx>

#endif

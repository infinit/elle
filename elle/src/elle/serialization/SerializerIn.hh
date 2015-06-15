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
      SerializerIn(std::istream& input,
                   bool versioned);
      SerializerIn(std::istream& input,
                   elle::Version version,
                   bool versioned);

    /*--------.
    | Helpers |
    `--------*/
    public:
      template <typename T>
      T
      deserialize(std::string const& name);

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

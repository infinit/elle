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
    /*------.
    | Types |
    `------*/
    public:
      typedef SerializerOut Self;
      typedef Serializer Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      SerializerOut(std::ostream& output);
      SerializerOut(std::ostream& output, elle::Version version);

    /*--------.
    | Details |
    `--------*/
    protected:
      std::ostream&
      output();
      ELLE_ATTRIBUTE(std::ostream&, output);
    };
  }
}

# include <elle/serialization/Serializer.hxx>

#endif

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
      SerializerOut(std::ostream& output,
                    bool versioned);
      SerializerOut(std::ostream& output,
                    elle::Version version,
                    bool versioned);

    /*----------------.
    | Const overloads |
    `----------------*/
    public:
      template <typename T>
      void
      serialize_forward(T const& v);
      template <typename T>
      void
      serialize(std::string const& name, T const& v);

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

# include <elle/serialization/SerializerOut.hxx>

#endif

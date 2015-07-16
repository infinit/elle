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

    /*----------.
    | Overloads |
    `----------*/
    public:
      template <typename T>
      void
      serialize_forward(T const& v);
      template <typename T>
      void
      serialize(std::string const& name, T const& v);
      template <typename As,
                template <typename, typename> class C, typename T, typename A>
      void
      serialize(std::string const& name, C<T, A>& collection, as<As>);
    protected:
      template <typename As,
                template <typename, typename> class C, typename T, typename A>
      void
      _serialize(std::string const& name,
                 C<T, A>& collection,
                 as<As>);

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

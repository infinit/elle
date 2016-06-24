#ifndef ELLE_SERIALIZATION_SERIALIZER_OUT_HH
# define ELLE_SERIALIZATION_SERIALIZER_OUT_HH

# include <iosfwd>

# include <elle/attribute.hh>
# include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    class ELLE_API SerializerOut
      : public Serializer
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
      SerializerOut(std::ostream& output, bool versioned = true);
      SerializerOut(std::ostream& output,
                    Versions versions, bool versioned = true);

    /*----------.
    | Overloads |
    `----------*/
    public:
      // Const overloads
      template <typename T>
      void
      serialize_forward(T const& v);
      template <typename T>
      void
      serialize_forward(T const* &v);
      template <typename T>
      void
      serialize(std::string const& name, T const& v);
      template <typename T1, typename T2>
      void
      serialize(std::pair<T1, T2> const& p);
      // Litteral strings overload
      void
      serialize(std::string const& name, char const* v);
      template <typename As,
                template <typename, typename> class C, typename T, typename A>
      void
      serialize(std::string const& name, C<T, A>& collection, as<As>);
      template <typename T>
      void
      serialize_ptr(std::string const& name, T* ptr);
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
      friend class Serializer;
      std::ostream&
      output();
      ELLE_ATTRIBUTE(std::ostream&, output);
    };
  }
}

# include <elle/serialization/SerializerOut.hxx>

#endif

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
      using Self = elle::serialization::SerializerOut;
      using Super = elle::serialization::Serializer;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      SerializerOut(bool versioned = true);
      SerializerOut(Versions versions, bool versioned = true);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      bool
      out() const override;

    /*----------.
    | Overloads |
    `----------*/
    public:
      // Const overloads
      template <typename S = void, typename T>
      void
      serialize_forward(T const& v);
      template <typename S = void, typename T>
      void
      serialize_forward(T const* &v);
      template <typename S = void, typename T>
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
      template <typename S, typename T>
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
    };
  }
}

# if not defined(ELLE_SERIALIZATION_SERIALIZER_HXX)
#  include <elle/serialization/SerializerOut.hxx>
# endif

#endif

#ifndef ELLE_SERIALIZATION_SERIALIZER_IN_HH
# define ELLE_SERIALIZATION_SERIALIZER_IN_HH

# include <iosfwd>

# include <elle/attribute.hh>
# include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    template <typename T> struct is_nullable;

    class ELLE_API SerializerIn
      : public Serializer
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
      SerializerIn(std::istream& input, bool versioned);
      SerializerIn(std::istream& input,
                   Versions versions, bool versioned = true);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      bool
      out() const override;

    /*--------.
    | Helpers |
    `--------*/
    public:
      template <typename T, typename Serializer = void>
      std::enable_if_t<
        !std::is_base_of<boost::optional_detail::optional_tag, T>::value
        && !is_nullable<T>::value,
        T>
      deserialize(std::string const& name);

      template <typename T, typename Serializer = void>
      std::enable_if_t<
        std::is_base_of<boost::optional_detail::optional_tag, T>::value
        || is_nullable<T>::value,
        T>
      deserialize(std::string const& name);

      template <typename T, typename Serializer = void>
      T
      deserialize();

    /*--------.
    | Details |
    `--------*/
    protected:
      friend class Serializer;
      ELLE_ATTRIBUTE_R(std::istream&, input);
    };
  }
}

# include <elle/serialization/Serializer.hxx>

#endif

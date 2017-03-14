// Cppguard needed.
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
      using Self = elle::serialization::SerializerIn;
      using Super = elle::serialization::Serializer;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      SerializerIn(bool versioned);
      SerializerIn(Versions versions, bool versioned = true);

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
    };
  }
}

# include <elle/serialization/Serializer.hxx>

#endif

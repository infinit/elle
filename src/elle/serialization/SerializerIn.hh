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
    template <typename T>
    struct is_nullable;

    /// A Serializer specialized for reading data from a stream and
    /// deserializing object.
    class ELLE_API SerializerIn
      : public Serializer
    {
    /*------.
    | Types |
    `------*/
    public:
      using Self = SerializerIn;
      using Super = Serializer;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Construct a SerializerIn from an input stream.
      ///
      /// @see Serializer(bool);
      SerializerIn(bool versioned);
      /// Construct a SerializerIn from an input stream.
      ///
      /// @see Serializer(Versions, bool);
      SerializerIn(Versions versions, bool versioned = true);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      /// Return false.
      bool
      out() const override;

    /*--------.
    | Helpers |
    `--------*/
    public:
      /// Deserialize a named object of type T.
      ///
      /// @tparam T The type of the object to deserialize.
      /// @tparam Serializer The type of Serializer to use.
      /// @param name The name of the entry.
      /// @returns An object of type T.
      template <typename T, typename Serializer = void>
      std::enable_if_t<
        !std::is_base_of<boost::optional_detail::optional_tag, T>::value
        && !is_nullable<T>::value,
        T>
      deserialize(std::string const& name);
      /// Deserialize a named object of type T.
      ///
      /// @tparam T The type of the object to deserialize.
      /// @tparam Serializer The type of Serializer to use.
      /// @param name The name of the entry.
      /// @returns An object of type T.
      template <typename T, typename Serializer = void>
      std::enable_if_t<
        std::is_base_of<boost::optional_detail::optional_tag, T>::value
        || is_nullable<T>::value,
        T>
      deserialize(std::string const& name);
      /// Deserialize an anonymous object of type T.
      ///
      /// @tparam T The type of the object to deserialize.
      /// @tparam Serializer The type of Serializer to use.
      /// @returns An object of type T.
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

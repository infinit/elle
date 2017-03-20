#ifndef ELLE_SERIALIZATION_SERIALIZER_OUT_HH
# define ELLE_SERIALIZATION_SERIALIZER_OUT_HH

# include <iosfwd>

# include <elle/attribute.hh>
# include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    /// A Serializer specialized for serializing and writing serialized data to
    /// a stream.
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
      /// Construct a SerializerOut from an output stream.
      ///
      /// @see Serializer(bool);
      SerializerOut(bool versioned = true);
      /// Construct a SerializerOut from an output stream.
      ///
      /// @see Serializer(Versions, bool);
      SerializerOut(Versions versions, bool versioned = true);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      /// Return true.
      bool
      out() const override;

    /*----------.
    | Overloads |
    `----------*/
    public:
      /// The SerializerOut doesn't modify the given object.

      /// Const overload of Serializer::serialize_forward.
      ///
      /// @see Serializer::serialize_forward(T&).
      template <typename S = void, typename T>
      void
      serialize_forward(T const& v);
      /// Const overload of Serializer::serialize_forward.
      ///
      /// @see Serializer::serialize_forward(T*).
      template <typename S = void, typename T>
      void
      serialize_forward(T const* &v);
      /// Const overload of Serializer::serialize.
      ///
      /// @see Serializer::serialize(..., T&);
      template <typename S = void, typename T>
      void
      serialize(std::string const& name, T const& v);
      /// Const overload of Serializer::serialize.
      ///
      /// @see Serializer::serialize(std::pair<T1, T2>&);
      template <typename T1, typename T2>
      void
      serialize(std::pair<T1, T2> const& p);
      /// Serialize literal strings.
      ///
      /// @param name The name of the entry.
      /// @param v The literal string to write.
      void
      serialize(std::string const& name, char const* v);
      /// Overload of Serializer::serialize.
      ///
      /// @see Serializer::serialize(std::string, collection&);x
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
      friend class Serializer;
    };
  }
}

# if not defined(ELLE_SERIALIZATION_SERIALIZER_HXX)
#  include <elle/serialization/SerializerOut.hxx>
# endif

#endif

#ifndef ELLE_SERIALIZATION_SERIALIZER_HH
# define ELLE_SERIALIZATION_SERIALIZER_HH

# include <functional>
# include <list>
# include <memory>
# include <string>
# include <unordered_map>

# include <boost/optional.hpp>
# include <boost/date_time/posix_time/posix_time.hpp>

# include <elle/Buffer.hh>
# include <elle/Version.hh>
# include <elle/serialization/fwd.hh>
# include <elle/sfinae.hh>

namespace elle
{
  namespace serialization
  {
    template <typename T>
    class as;

    class VirtuallySerializable
    {
    public:
      virtual
      ~VirtuallySerializable()
      {}

      virtual
      void
      serialize(Serializer& s) = 0;

      static constexpr char const* virtually_serializable_key = ".type";
    };

    template <typename T>
    struct Serialize
    {};

    class Serializer
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Serializer Self;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Serializer(bool versioned);
      Serializer(elle::Version version, bool versioned);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      bool
      in() const;
      bool
      out() const;
      ELLE_ATTRIBUTE_R(elle::Version const, version);
      ELLE_ATTRIBUTE_R(bool, versioned);
    private:
      bool
      _out() const;

    /*--------------.
    | Serialization |
    `--------------*/
    public:
      template <typename T>
      void
      serialize(std::string const& name, T& v);
      template <typename T>
      void
      serialize(std::string const& name, boost::optional<T>& opt);
      template <typename T>
      void
      serialize(std::string const& name, std::unique_ptr<T>& opt);
      template <typename T>
      void
      serialize(std::string const& name, std::shared_ptr<T>& opt);
      template <typename T, typename A>
      void
      serialize(std::string const& name, T& v, as<A>);
      template <typename T>
      void
      serialize_object(std::string const& name, T& v);
      template <typename T>
      void
      serialize_pod(std::string const& name, T& v);
      template <typename T>
      void
      serialize_forward(T& v);

    protected:
      virtual
      bool
      _enter(std::string const& name);
      virtual
      void
      _leave(std::string const& name);
      virtual
      void
      _serialize_array(std::string const& name,
                       std::function<void ()> const& f) = 0;
      virtual
      void
      _serialize(std::string const& name, int64_t& v) = 0;
      virtual
      void
      _serialize(std::string const& name, uint64_t& v) = 0;
      virtual
      void
      _serialize(std::string const& name, int32_t& v) = 0;
      virtual
      void
      _serialize(std::string const& name, uint32_t& v) = 0;
      virtual
      void
      _serialize(std::string const& name, int8_t& v) = 0;
      virtual
      void
      _serialize(std::string const& name, uint8_t& v) = 0;
      virtual
      void
      _serialize(std::string const& name, double& v) = 0;
      virtual
      void
      _serialize(std::string const& name, bool& v) = 0;
      virtual
      void
      _serialize(std::string const& name, std::string& v) = 0;
      virtual
      void
      _serialize(std::string const& name, elle::Buffer& v) = 0;
      void
      _serialize(std::string const& name, elle::WeakBuffer& v);
      virtual
      void
      _serialize(std::string const& name, boost::posix_time::ptime& v) = 0;
      virtual
      void
      _serialize_option(std::string const& name,
                        bool present,
                        std::function<void ()> const& f) = 0;
      template <template <typename, typename> class C, typename T, typename A>
      void
      _serialize(std::string const& name, C<T, A>& collection);
      template <typename T1, typename T2>
      void
      _serialize(std::string const& name, std::pair<T1, T2>& collection);
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::string const& name,
                std::unordered_map<K, V, Rest...>& map);
    private:
      template <typename T>
      void
      _serialize_anonymous(std::string const& name, T& v);
      template <typename T>
      void
      _serialize_anonymous(std::string const& name, std::unique_ptr<T>& v);
      template <typename T>
      friend struct Serialize;
      class Details;
      friend class Details;
    };

    template <typename T>
    class as
    {};

    template <typename T>
    class Hierarchy;
  }
}

# if !defined(ELLE_SERIALIZATION_SERIALIZER_IN_HH) && !defined(ELLE_SERIALIZATION_SERIALIZER_OUT_HH)
#  include <elle/serialization/Serializer.hxx>
# endif

#endif

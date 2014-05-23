#ifndef ELLE_SERIALIZATION_SERIALIZER_HH
# define ELLE_SERIALIZATION_SERIALIZER_HH

# include <functional>
# include <list>
# include <memory>
# include <string>
# include <unordered_map>

# include <boost/optional.hpp>

# include <elle/Buffer.hh>
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
    };

    class Serializer
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Serializer Self;

    /*-----------.
    | Properties |
    `-----------*/
    public:
      bool
      in() const;
      bool
      out() const;
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
      void
      serialize_virtual_object(std::string const& name, VirtuallySerializable& v);
      template <typename T>
      void
      serialize_pod(std::string const& name, T& v);

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

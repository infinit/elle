#ifndef ELLE_SERIALIZATION_SERIALIZER_HH
# define ELLE_SERIALIZATION_SERIALIZER_HH

# include <functional>
# include <list>
# include <memory>
# include <set>
# include <string>
# include <type_traits>
# include <typeinfo>
# include <unordered_map>
# include <unordered_set>

# include <boost/any.hpp>
# include <boost/date_time/posix_time/posix_time.hpp>
# include <boost/multi_index_container.hpp>

# include <elle/Buffer.hh>
# include <elle/TypeInfo.hh>
# include <elle/Version.hh>
# include <elle/optional.hh>
# include <elle/serialization/fwd.hh>
# include <elle/sfinae.hh>

namespace elle
{
  namespace serialization
  {
    template <typename T>
    class as;

    template <typename T>
    struct Serialize
    {};

    class Context
    {
    public:
      template <typename T>
      void
      set(T value)
      {
        this->_value[type_info<T>()] = std::move(value);
      }

      template <typename T>
      void
      get(T& value)
      {
        auto ti = type_info<T>();
        auto it = this->_value.find(ti);
        if (it == this->_value.end())
          throw Error(
            elle::sprintf("missing serialization context for %s", ti.name()));
        value = boost::any_cast<T>(it->second);
      }

      template <typename T>
      void
      get(T& value, const T& default_value)
      {
        auto ti = type_info<T>();
        auto it = this->_value.find(ti);
        if (it == this->_value.end())
          value = default_value;
        else
          value = boost::any_cast<T>(it->second);
      }

      template <typename T>
      bool has() const
      {
        auto ti = type_info<T>();
        auto it = this->_value.find(ti);
        return it != this->_value.end();
      }

      Context&
      operator += (Context const& source)
      {
        for (auto const& e: source._value)
          this->_value.insert(e);
        return *this;
      }

      size_t
      size() const
      {
        return this->_value.size();
      }

      ELLE_ATTRIBUTE((std::map<TypeInfo, boost::any>), value);
    };

    class Serializer
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Serializer Self;
      typedef std::unordered_map<TypeInfo, Version> Versions;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Serializer(bool versioned);
      Serializer(Versions versions,bool versioned);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      bool
      in() const;
      bool
      out() const;
      bool
      text() const;
      ELLE_ATTRIBUTE_R(bool, versioned);
      ELLE_ATTRIBUTE_R(boost::optional<Versions>, versions);
    private:
      bool
      _out() const;

    /*--------------.
    | Serialization |
    `--------------*/
    public:
      template <typename Serializer = void, typename T>
      void
      serialize(std::string const& name, T& v);
      template <typename T>
      void
      serialize(std::string const& name, boost::optional<T>& opt);
      template <typename T, typename D>
      void
      serialize(std::string const& name, std::unique_ptr<T, D>& opt);
      template <typename T>
      void
      serialize(std::string const& name, std::shared_ptr<T>& opt);
      template <typename T>
      typename std::enable_if<
        !_details::has_serialize_convert_api<T*, void>(), void>::type
      serialize(std::string const& name, T*& opt);
      template <typename T, typename As>
      void
      serialize(std::string const& name, T& v, as<As>);
      template <typename As,
                template <typename, typename> class C,
                typename T,
                typename A>
      typename
      std::enable_if<std::is_default_constructible<T>::value, void>::type
      serialize(std::string const& name, C<T, A>& collection, as<As>);
      template <typename T>
      void
      serialize_object(std::string const& name, T& v);
      void
      serialize_object(std::string const& name, elle::Version& v);
      template <typename T>
      void
      serialize_pod(std::string const& name, T& v);
      template <typename T>
      void
      serialize_ptr(std::string const& name, T* &v, bool anonymous = false);
      template<typename T>
      void
      serialize_with_struct_serialize(
                                      std::string const& name,
                                      T& v,
                                      bool anonymous);
      template <typename T>
      void
      serialize_forward(T& v);
      template <typename Serializer, typename T>
      void
      serialize_forward(T& v);
      template <typename T>
      void
      serialize_context(T& value);
      template <typename T>
      void
      set_context(T value);
      void
      set_context(Context const& context);
      ELLE_ATTRIBUTE_R(Context, context);

    /*------------.
    | Enter/leave |
    `------------*/
    public:
      class Entry
      {
      public:
        ~Entry();
        operator bool() const;
      private:
        Entry(Serializer& s, std::string const& name);
        friend class Serializer;
        ELLE_ATTRIBUTE(Serializer&, serializer);
        ELLE_ATTRIBUTE(std::string const&, name);
        ELLE_ATTRIBUTE(bool, entered);
      };
      Entry
      enter(std::string const& name);
    protected:
      virtual
      bool
      _enter(std::string const& name);
      virtual
      void
      _leave(std::string const& name);
      virtual
      void
      _size(int size);
      virtual
      bool
      _text() const;
      virtual
      void
      _serialize_array(std::string const& name,
                       int size, // -1 for in(), array size for out()
                       std::function<void ()> const& f) = 0;
      virtual
      void
      _serialize_dict_key(std::string const& name,
                      std::function<void ()> const& f);
      virtual
      void
      _deserialize_dict_key(
        std::function<void (std::string const&)> const& f);
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
      _serialize(std::string const& name, int16_t& v) = 0;
      virtual
      void
      _serialize(std::string const& name, uint16_t& v) = 0;
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
      template <typename Repr, typename Ratio>
      void
      _serialize(std::string const& name,
                 std::chrono::duration<Repr, Ratio>& duration);
      virtual
      void
      _serialize_time_duration(std::int64_t& ticks,
                               std::int64_t& num,
                               std::int64_t& denom) = 0;
      virtual
      void
      _serialize_named_option(std::string const& name,
                              bool present,
                              std::function<void ()> const& f) = 0;
      virtual
      void
      _serialize_option(std::string const& name,
                        bool present,
                        std::function<void ()> const& f) = 0;
      template <template <typename, typename> class C, typename T, typename A>
      void
      _serialize(std::string const& name, C<T, A>& collection);
      template <typename C>
      void
      _serialize_collection(std::string const& name, C& collection);
      template <typename T, typename A>
      void
      _serialize(std::string const& name, std::vector<T, A>& collection);
      template <typename T, typename C, typename A>
      void
      _serialize(std::string const& name, std::set<T, C, A>& collection);
      template <typename T, typename I>
      void
      _serialize(std::string const& name,
                 boost::multi_index::multi_index_container<T, I>& collection);
      template <typename As,
                template <typename, typename> class C, typename T, typename A>
      typename
      std::enable_if<std::is_default_constructible<T>::value, void>::type
      _serialize(std::string const& name, C<T, A>& collection, as<As>);
      template <typename T1, typename T2>
      void
      _serialize(std::string const& name, std::pair<T1, T2>& collection);
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::string const& name,
                std::unordered_map<K, V, Rest...>& map);
      template <typename V, typename ... Rest>
      void
      _serialize(std::string const& name,
                 std::unordered_map<std::string, V, Rest...>& map);
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::string const& name,
                 std::map<K, V, Rest...>& map);
      template <typename V, typename ... Rest>
      void
      _serialize(std::string const& name,
                 std::map<std::string, V, Rest...>& map);
      template <typename V, typename ... Rest>
      void
      _serialize(std::string const& name,
                std::unordered_set<V, Rest...>& set);
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::string const& name,
                std::unordered_multimap<K, V, Rest...>& map);
      template <typename C>
      void
      _serialize_assoc(std::string const& name, C& map);
      template <typename S = void, typename T>
      void
      _serialize_anonymous(std::string const& name, T& v);
      template <typename S = void>
      typename std::enable_if<std::is_same<S, void>::value, void>::type
      _serialize_anonymous(std::string const& name, std::exception_ptr& e);
      void
      _serialize_anonymous_exception(
        std::string const& name, std::exception_ptr& e);
      template <typename C>
      typename std::enable_if_exists<
        decltype(
          std::declval<C>().emplace(
            std::declval<elle::serialization::SerializerIn>())),
        void>::type
      _deserialize_in_array(std::string const& name, C& collection);
      template <typename C>
      typename std::enable_if_exists<
        decltype(
          std::declval<C>().emplace_back(
            std::declval<elle::serialization::SerializerIn>())),
        void>::type
      _deserialize_in_array(std::string const& name, C& collection);
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

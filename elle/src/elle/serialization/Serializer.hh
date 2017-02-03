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
# include <elle/attribute.hh>
# include <elle/err.hh>
# include <elle/log.hh>
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

    class ELLE_API Context
    {
    public:
      template <typename... Args>
      Context(Args&&... args)
      {
        using swallow = int[];
        (void) swallow
        {
          (this->set(std::forward<Args>(args)), 0)...
        };
      }

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
          elle::err("missing serialization context for %s", ti.name());
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

    class ELLE_API Serializer
    {
    /*------.
    | Types |
    `------*/
    public:
      using Self = Serializer;
      using Versions = std::unordered_map<TypeInfo, Version>;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Serializer(bool versioned);
      Serializer(Versions versions, bool versioned);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      /// Whether is a SerializerIn.
      bool
      in() const;
      /// Whether is a SerializerOut.
      virtual
      bool
      out() const = 0;
      bool
      text() const;
      ELLE_ATTRIBUTE_R(bool, versioned);
      ELLE_ATTRIBUTE_R(boost::optional<Versions>, versions);

    /*--------------.
    | Serialization |
    `--------------*/
    public:
      template <typename Serializer = void, typename T>
      void
      serialize(std::string const& name, T& v);
      template <typename Serializer = void, typename T>
      void
      serialize(std::string const& name, boost::optional<T>& opt);
      template <typename Serializer = void, typename T, typename D>
      void
      serialize(std::string const& name, std::unique_ptr<T, D>& opt);
      template <typename Serializer = void, typename T>
      void
      serialize(std::string const& name, std::shared_ptr<T>& opt);
      template <typename Serializer = void, typename T>
      std::enable_if_t<
        !_details::has_serialize_convert_api<T*, void>(), void>
      serialize(std::string const& name, T*& opt);
      template <typename Serializer = void, typename T, typename As>
      void
      serialize(std::string const& name, T& v, as<As>);
      template <typename As,
                template <typename, typename> class C,
                typename T,
                typename A>
      std::enable_if_t<std::is_default_constructible<T>::value, void>
      serialize(std::string const& name, C<T, A>& collection, as<As>);
      template <typename T>
      void
      serialize_object(T& v);
      void
      serialize_object(elle::Version& v);
      template <typename T>
      void
      serialize_ptr(std::string const& name, T* &v, bool anonymous = false);
      template<typename T>
      void
      serialize_with_struct_serialize(std::string const& name,
                                      T& v,
                                      bool anonymous);
      template <typename Serializer = void, typename T>
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
      template <typename S = void, typename Serializer, typename T>
      static
      void
      serialize_switch(Serializer& s, T& v);

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
        ELLE_ATTRIBUTE(elle::log::detail::Send, log);
        ELLE_ATTRIBUTE(std::string const&, name);
        ELLE_ATTRIBUTE(bool, entered);
      };
      Entry
      enter(std::string const& name);
      std::string
      current_name() const;
    protected:
      virtual
      bool
      _enter(std::string const& name);
      virtual
      void
      _leave(std::string const& name);
      ELLE_ATTRIBUTE(std::vector<std::string>, names, protected);

    protected:
      virtual
      void
      _size(int size);
      virtual
      bool
      _text() const;
      virtual
      void
      _serialize_array(int size, // -1 for in(), array size for out()
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
      _serialize(int64_t& v) = 0;
      virtual
      void
      _serialize(uint64_t& v) = 0;
      virtual
      void
      _serialize(int32_t& v) = 0;
      virtual
      void
      _serialize(uint32_t& v) = 0;
      virtual
      void
      _serialize(int16_t& v) = 0;
      virtual
      void
      _serialize(uint16_t& v) = 0;
      virtual
      void
      _serialize(int8_t& v) = 0;
      virtual
      void
      _serialize(uint8_t& v) = 0;
      virtual
      void
      _serialize(double& v) = 0;
      virtual
      void
      _serialize(bool& v) = 0;
      virtual
      void
      _serialize(std::string& v) = 0;
      virtual
      void
      _serialize(elle::Buffer& v) = 0;
      void
      _serialize(elle::WeakBuffer& v);
      virtual
      void
      _serialize(boost::posix_time::ptime& v) = 0;
      template <typename Repr, typename Ratio>
      void
      _serialize(std::chrono::duration<Repr, Ratio>& duration);
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
      _serialize_option(bool present,
                        std::function<void ()> const& f) = 0;
      template <typename S = void,
                template <typename, typename> class C,
                typename T, typename A>
      void
      _serialize(C<T, A>& collection);
      template <typename S = void, typename C>
      void
      _serialize_collection(C& collection);
      template <typename S = void, typename T, typename A>
      void
      _serialize(std::vector<T, A>& collection);
      template <typename T, typename C, typename A>
      void
      _serialize(std::set<T, C, A>& collection);
      template <typename T, typename I>
      void
      _serialize(boost::multi_index::multi_index_container<T, I>& collection);
      template <typename As,
                template <typename, typename> class C, typename T, typename A>
      std::enable_if_t<std::is_default_constructible<T>::value, void>
      _serialize(C<T, A>& collection, as<As>);
      template <typename T1, typename T2>
      void
      _serialize(std::pair<T1, T2>& collection);
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::unordered_map<K, V, Rest...>& map);
      template <typename V, typename ... Rest>
      void
      _serialize(std::unordered_map<std::string, V, Rest...>& map);
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::map<K, V, Rest...>& map);
      template <typename V, typename ... Rest>
      void
      _serialize(std::map<std::string, V, Rest...>& map);
      template <typename V, typename ... Rest>
      void
      _serialize(std::unordered_set<V, Rest...>& set);
      template <typename K, typename V, typename ... Rest>
      void
      _serialize(std::unordered_multimap<K, V, Rest...>& map);
      template <typename C>
      void
      _serialize_assoc(C& map);
      void
      _serialize_anonymous_exception(std::exception_ptr& e);
      template <typename S = void, typename C>
      std::enable_if_exists_t<
        decltype(
          std::declval<C>().emplace(
            std::declval<elle::serialization::SerializerIn>())),
        void>
      _deserialize_in_array(C& collection);
      template <typename S = void, typename C>
      std::enable_if_exists_t<
        decltype(
          std::declval<C>().emplace_back(
            std::declval<elle::serialization::SerializerIn>())),
        void>
      _deserialize_in_array(C& collection);
      template <typename T>
      friend struct Serialize;
      class Details;
      friend class Details;
    };

    template <typename T>
    class as
    {};

    template <typename T>
    class ELLE_API Hierarchy;
  }
}

# if !defined(ELLE_SERIALIZATION_SERIALIZER_IN_HH) && !defined(ELLE_SERIALIZATION_SERIALIZER_OUT_HH)
#  include <elle/serialization/Serializer.hxx>
# endif

#endif

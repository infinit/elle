#ifndef ELLE_SERIALIZATION_SERIALIZER_HXX
# define ELLE_SERIALIZATION_SERIALIZER_HXX

# include <boost/optional.hpp>

# include <elle/Backtrace.hh>
# include <elle/ScopedAssignment.hh>
# include <elle/TypeInfo.hh>
# include <elle/finally.hh>
# include <elle/serialization/Error.hh>
# include <elle/serialization/SerializerIn.hh>
# include <elle/serialization/SerializerOut.hh>
# include <elle/utils.hh>

namespace elle
{
  namespace serialization
  {
    /*--------.
    | Details |
    `--------*/

    namespace _details
    {
      template <typename T, typename API = void>
      struct serialization_api
        : public API
      {};

      template <typename T>
      struct serialization_api<T, void>
        : public elle::serialization::Serialize<T>
      {};

# define ELLE_SERIALIZATION_STATIC_PREDICATE(Name, Test)                \
  template <typename T, typename S>                                     \
  inline constexpr                                                      \
  typename std::enable_if_exists<ELLE_ATTRIBUTE_STRIP_PARENS(Test),     \
                                 bool>::type                            \
  BOOST_PP_CAT(_, Name)(int)                                            \
  {                                                                     \
    return true;                                                        \
  }                                                                     \
                                                                        \
  template <typename T, typename S>                                     \
  inline constexpr                                                      \
  bool                                                                  \
  BOOST_PP_CAT(_, Name)(...)                                            \
  {                                                                     \
    return false;                                                       \
  }                                                                     \
                                                                        \
  template <typename T, typename S>                                     \
  inline constexpr                                                      \
  bool                                                                  \
  Name()                                                                \
  {                                                                     \
    return BOOST_PP_CAT(_, Name)<T, S>(42);                             \
  }                                                                     \

      ELLE_STATIC_PREDICATE(
        has_version_tag,
        decltype(T::serialization_tag::version));
      ELLE_SERIALIZATION_STATIC_PREDICATE(
        has_serialize_convert_api,
        (typename serialization_api<T, S>::Type));
      ELLE_STATIC_PREDICATE(
        has_serialize_wrapper_api,
        typename elle::serialization::Serialize<T>::Wrapper);
      ELLE_SERIALIZATION_STATIC_PREDICATE(
        has_serialize_functions_api,
        (std::pair<
           decltype(serialization_api<T, S>::serialize(
                      std::declval<T>(),
                      std::declval<elle::serialization::SerializerOut&>())),
           decltype(serialization_api<T, S>::deserialize(
                      std::declval<elle::serialization::SerializerIn&>()))>));

      template <typename T>
      typename std::enable_if<has_version_tag<T>(), elle::Version>::type
      version_tag(boost::optional<Serializer::Versions> const& versions)
      {
        ELLE_LOG_COMPONENT("elle.serialization.Serializer");
        if (versions)
        {
          auto it = versions->find(type_info<typename T::serialization_tag>());
          if (it != versions->end())
          {
            ELLE_DUMP("use local serialization version for %s",
                      elle::type_info<T>());
            return it->second;
          }
        }
        ELLE_DUMP("use default serialization version for %s",
                  elle::type_info<T>());
        return T::serialization_tag::version;
      }

      template <typename T>
      typename std::enable_if<!has_version_tag<T>(), elle::Version>::type
      version_tag(boost::optional<Serializer::Versions> const& versions)
      {
        ELLE_LOG_COMPONENT("elle.serialization.Serializer");
        ELLE_WARN("no serialization version tag for %s", elle::type_info<T>());
        ELLE_ABORT("no serialization version tag for %s", elle::type_info<T>());
        throw elle::Error(elle::sprintf("no serialization version tag for %s",
                                        elle::type_info<T>()));
      }

      // option_reset: reset boost::optional, smart pointers and raw pointers

      template <typename T>
      void
      option_reset(T& opt)
      {
        opt.reset();
      }

      template <typename T>
      void
      option_reset(T*& opt)
      {
        opt = nullptr;
      }

      template <typename P, typename T>
      void _set_ptr(P& target, T* ptr)
      {
        target.reset(ptr);
      }

      template<typename P, typename T>
      void _set_ptr(P* &target, T* ptr)
      {
        target = ptr;
      }
    }

    class Serializer::Details
    {
    public:
      template <typename P, typename T>
      static
      typename std::enable_if<
        std::is_base_of<VirtuallySerializableBase, T>::value,
        void
      >::type
      _smart_virtual_switch(
        Serializer& s,
        std::string const& name,
        P& ptr)
      {
        if (s.out())
        {
          ELLE_ASSERT(bool(ptr));
          auto id = elle::type_info(*ptr);
          auto const& map = Hierarchy<T>::_rmap();
          auto it = map.find(id);
          if (it == map.end())
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            auto message =
              elle::sprintf("unknown serialization type: %s", id);
            ELLE_WARN("%s", message);
            throw Error(message);
          }
          auto type_name = it->second;
          s.serialize(T::virtually_serializable_key, type_name);
          s.serialize_object(name, *ptr);
        }
        else
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DEBUG_SCOPE("%s: deserialize virtual key %s of type %s",
                           s, name, type_info<T>());
          auto const& map = Hierarchy<T>::_map();
          std::string type_name;
          s.serialize(T::virtually_serializable_key, type_name);
          ELLE_DUMP("%s: type: %s", s, type_name);
          auto it = map.find(type_name);
          if (it == map.end())
            throw Error(elle::sprintf("unknown deserialization type: \"%s\"",
                                      type_name));
          _details::_set_ptr(
            ptr, it->second(static_cast<SerializerIn&>(s)).release());
        }
      }

      template <typename P, typename T>
      static
      typename std::enable_if<
        !std::is_base_of<VirtuallySerializableBase, T>::value,
        void
      >::type
      _smart_virtual_switch(
        Serializer& s,
        std::string const& name,
        P& ptr)
      {
        if (s.in())
          _details::_set_ptr(
            ptr,
            new T(Details::deserialize<T>(static_cast<SerializerIn&>(s), 42)));
        else
          s._serialize_anonymous(name, *ptr);
      }

      template <typename T, typename Serializer = void>
      static
      typename std::enable_if<
        !std::is_base_of<boost::optional_detail::optional_tag, T>::value &&
        std::is_constructible<T, SerializerIn&, elle::Version const&>::value,
        T>::type
      deserialize(SerializerIn& self, int)
      {
        ELLE_LOG_COMPONENT("elle.serialization.Serializer");
        auto version = _details::version_tag<T>(self.versions());
        if (self.versioned())
        {
          {
            ELLE_TRACE_SCOPE("serialize version: %s", version);
            self.serialize(".version", version);
          }
        }
        return T(self, version);
      }

      template <typename T, typename Serializer = void>
      static
      typename std::enable_if<
        !std::is_base_of<boost::optional_detail::optional_tag, T>::value &&
        std::is_constructible<T, SerializerIn&>::value,
        T>::type
      deserialize(SerializerIn& self, int)
      {
        ELLE_LOG_COMPONENT("elle.serialization.Serializer");
        if (self.versioned())
        {
          auto version = _details::version_tag<T>(self.versions());
          {
            ELLE_TRACE_SCOPE("serialize version: %s", version);
            self.serialize(".version", version);
          }
        }
        return T(self);
      }

      template <typename T>
      struct is_pair
      {
        static constexpr bool value = false;
      };

      template <typename T1, typename T2>
      struct is_pair<std::pair<T1, T2>>
      {
        static constexpr bool value = true;
      };

      template <typename T, typename Serializer = void>
      static
      typename std::enable_if<is_pair<T>::value, T>::type
      deserialize(SerializerIn& self, int)
      {
        typedef typename T::first_type T1;
        typedef typename T::second_type T2;
        int i = 0;
        boost::optional<T1> first;
        boost::optional<T2> second;
        self._serialize_array(
          "",
          -1,
          [&] ()
          {
            if (i == 0)
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("%s: deserialize first member", self);
              first.emplace(self.deserialize<T1>());
            }
            else if (i == 1)
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("%s: deserialize second member", self);
              second.emplace(self.deserialize<T2>());
            }
            else
              throw Error(elle::sprintf(
                            "too many values to unpack for a pair: %s", i));
            ++i;
          });
        if (!first || !second)
          throw Error(elle::sprintf(
                        "too few value to unpack for a pair: %s", i));
        return std::pair<T1, T2>(std::move(first.get()),
                                 std::move(second.get()));
      }

      template <typename T, typename Serializer = void>
      static
      typename
      std::enable_if<_details::has_serialize_functions_api<T, void>(), T>::type
      deserialize(SerializerIn& self, unsigned)
      {
        return
          _details::serialization_api<T, Serializer>::deserialize(self);
      }

      template <typename T, typename Serializer = void>
      static
      typename std::enable_if<_details::has_serialize_convert_api<T, void>(), T>::type
      deserialize(SerializerIn& self, unsigned)
      {
        // FIXME: convert_api could be factored with the next version if
        // _serialize_anonymous returned the deserialized object
        typename _details::serialization_api<T, Serializer>::Type value;
        self._serialize_anonymous("", value);
        return Serialize<T>::convert(value);
      }

      // This overload initializes PODs with "= {}" to avoid warnings.
      template <typename T, typename Serializer = void>
      static
      typename std::enable_if<
        std::is_pod<T>::value &&
        !_details::has_serialize_convert_api<T, void>() &&
        !_details::has_serialize_functions_api<T, void>(),
        T>::type
      deserialize(SerializerIn& self, unsigned)
      {
        T res = {};
        self._serialize_anonymous<Serializer>("", res);
        return res;
      }

      template <typename T, typename Serializer = void>
      static
      typename std::enable_if<
        !std::is_pod<T>::value &&
        !_details::has_serialize_convert_api<T, void>() &&
        !_details::has_serialize_functions_api<T, void>(),
        T>::type
      deserialize(SerializerIn& self, unsigned)
      {
        static_assert(
          std::is_base_of<boost::optional_detail::optional_tag, T>::value ||
          !std::is_constructible<T, elle::serialization::SerializerIn&>::value,
          "");
        T res;
        self._serialize_anonymous<Serializer>("", res);
        return res;
      }


      // Serialize boost::optionals and smart pointers
      template <typename T>
      static
      void
      serialize_named_option(Serializer& self, std::string const& name, T& opt);
      template <typename T>
      static
      void
      serialize_option(Serializer& self, std::string const& name,
                       T& opt, std::function<void ()> const& f);
    };

    template <typename T>
    struct Serialize<boost::optional<T>>
    {
      static
      void
      serialize(boost::optional<T> const& o,
                elle::serialization::SerializerOut& s)
      {
        s._serialize_option(
          "",
          bool(o),
          [&]
          {
            s._serialize_anonymous("", elle::unconst(*o));
          });
      }

      static
        boost::optional<T>
        deserialize(elle::serialization::SerializerIn& s)
      {
        boost::optional<T> res;
        s._serialize_option(
          "",
          false,
          [&]
          {
            res.emplace(Serializer::Details::deserialize<T>(
                          static_cast<SerializerIn&>(s), 42));
          });
        return res;
      }
    };

    template <typename T, typename D>
    struct Serialize<std::unique_ptr<T, D>>
    {
      static
      void
      serialize(std::unique_ptr<T, D> const& ptr,
                elle::serialization::SerializerOut& s)
      {
        s._serialize_option(
          "",
          bool(ptr),
          [&]
          {
            Serializer::Details::_smart_virtual_switch<std::unique_ptr<T, D>, T>
              (s, "SERIALIZE ANONYMOUS", elle::unconst(ptr));
          });
      }

      static
      std::unique_ptr<T, D>
      deserialize(elle::serialization::SerializerIn& s)
      {
        std::unique_ptr<T, D> ptr;
        s._serialize_option(
          "",
          true,
          [&]
          {
            Serializer::Details::_smart_virtual_switch<std::unique_ptr<T, D>, T>
              (s, "SERIALIZE ANONYMOUS", ptr);
          });
        return ptr;
      }
    };

    template <typename T>
    struct Serialize<std::shared_ptr<T>>
    {
      static
      void
      serialize(std::shared_ptr<T> const& ptr,
                elle::serialization::SerializerOut& s)
      {
        s._serialize_option(
          "",
          bool(ptr),
          [&]
          {
            Serializer::Details::_smart_virtual_switch<std::shared_ptr<T>, T>
              (s, "SERIALIZE ANONYMOUS", elle::unconst(ptr));
          });
      }

      static
      std::shared_ptr<T>
      deserialize(elle::serialization::SerializerIn& s)
      {
        std::shared_ptr<T> ptr;
        s._serialize_option(
          "",
          true,
          [&]
          {
            Serializer::Details::_smart_virtual_switch<std::shared_ptr<T>, T>
              (s, "SERIALIZE ANONYMOUS", ptr);
          });
        return ptr;
      }
    };

    template <typename T>
    struct Serialize<T*>
    {
      static
      void
      serialize(T* ptr,
                elle::serialization::SerializerOut& s)
      {
        s._serialize_option(
          "",
          bool(ptr),
          [&]
          {
            Serializer::Details::_smart_virtual_switch<T*, T>(s, "", ptr);
          });
      }

      static
      T*
      deserialize(elle::serialization::SerializerIn& s)
      {
        T* ptr = nullptr;
        s._serialize_option(
          "",
          true,
          [&]
          {
            Serializer::Details::_smart_virtual_switch<T*, T>(s, "", ptr);
          });
        return ptr;
      }
    };

    template <>
    struct Serialize<std::exception_ptr>
    {
      static
      void
      serialize(std::exception_ptr e,
                elle::serialization::SerializerOut& s)
      {
        s._serialize_anonymous_exception("", e);
      }

      static
      std::exception_ptr
      deserialize(elle::serialization::SerializerIn& s)
      {
        std::exception_ptr e;
        s._serialize_anonymous_exception("", e);
        return e;
      }
    };

    namespace
    {
      template <typename S, typename T>
      typename std::enable_if_exists<
        decltype(std::declval<T>().serialize(std::declval<Serializer&>())),
        void>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& obj,
        ELLE_SFINAE_IF_POSSIBLE())
      {
        s.serialize_object(name, obj);
      }

      template <typename S, typename T>
      typename std::enable_if_exists<
        decltype(std::declval<T>().serialize(
                   std::declval<Serializer&>(),
                   std::declval<elle::Version const&>())),
        void>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& obj,
        ELLE_SFINAE_IF_POSSIBLE())
      {
        s.serialize_object(name, obj);
      }

      template <typename S, typename T>
      void
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& v,
        ELLE_SFINAE_OTHERWISE())
      {
        s.serialize_pod(name, v);
      }

      template <typename S, typename T>
      typename std::enable_if<
        _details::has_serialize_convert_api<T, S>(), void>::type
      _serialize_switch(Serializer& s,
                        std::string const& name,
                        T& v,
                        ELLE_SFINAE_IF_POSSIBLE())
      {
        using API = typename _details::serialization_api<T, S>;
        using type = typename API::Type;
        if (s.out())
        {
          type value(Serialize<T>::convert(v));
          _serialize_switch<S, type>(s, name, value, ELLE_SFINAE_TRY());
        }
        else
        {
          type value;
          _serialize_switch<S, type>(s, name, value, ELLE_SFINAE_TRY());
          v = Serialize<T>::convert(value);
        }
      }

      template <typename S, typename T>
      typename std::enable_if<
        _details::has_serialize_wrapper_api<T>(), void>::type
      _serialize_switch(Serializer& s,
                        std::string const& name,
                        T& v,
                        ELLE_SFINAE_IF_POSSIBLE())
      {
        typedef typename Serialize<T>::Wrapper Wrapper;
        Wrapper wrapper(v);
        _serialize_switch<Wrapper>(s, name, wrapper, ELLE_SFINAE_TRY());
      }

      template <typename S, typename T>
      typename std::enable_if<
        _details::has_serialize_functions_api<T, S>(), void>::type
      _serialize_switch(Serializer& s,
                        std::string const& name,
                        T& v,
                        ELLE_SFINAE_IF_POSSIBLE())
      {
        if (s.out())
          _details::serialization_api<T, S>::serialize(
            v, static_cast<SerializerOut&>(s));
        else
          v = _details::serialization_api<T, S>::deserialize(
            static_cast<SerializerIn&>(s));
      }
    }

    /*--------------.
    | Serialization |
    `--------------*/

    template <typename S, typename T>
    void
    Serializer::serialize(std::string const& name, T& v)
    {
      static_assert(
        !std::is_base_of<VirtuallySerializableBase, T>::value,
        "serialize VirtuallySerializable objects through a pointer type");
      if (auto entry = this->enter(name))
        this->_serialize_anonymous<S>(name, v);
    }

    template <typename S, typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name, T& v)
    {
      static_assert(
        !std::is_base_of<VirtuallySerializableBase, T>::value,
        "serialize VirtuallySerializable objects through a pointer type");
      _serialize_switch<S>(*this, name, v, ELLE_SFINAE_TRY());
    }

    /*------------------------------------.
    | Serialization: Options and pointers |
    `------------------------------------*/

    template <typename T>
    void
    Serializer::Details::serialize_named_option(Serializer& self,
                                                std::string const& name,
                                                T& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      bool filled = false;
      self._serialize_named_option(
        name,
        bool(opt),
        [&]
        {
          ELLE_ENFORCE(self._enter(name));
          elle::SafeFinally leave([&] { self._leave(name); });
          self._serialize_anonymous(name, opt);
          filled = true;
        });
      if (!filled)
      {
        ELLE_DEBUG("reset option");
        _details::option_reset(opt);
      }
    }

    template <typename T>
    void
    Serializer::Details::serialize_option(Serializer& self,
                                          std::string const& name,
                                          T& opt,
                                          std::function<void ()> const& f)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      bool filled = false;
      self._serialize_option(
        name,
        bool(opt),
        [&]
        {
          filled = true;
          f();
        });
      if (!filled)
      {
        ELLE_DEBUG("reset option");
        _details::option_reset(opt);
      }
    }

    // boost::optional

    template <typename T>
    void
    Serializer::serialize(std::string const& name, boost::optional<T>& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize option \"%s\"", *this, name);
      Details::serialize_named_option(*this, name, opt);
    }

    // std::unique_ptr

    template <typename T, typename D>
    void
    Serializer::serialize(std::string const& name,
                          std::unique_ptr<T, D>& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize unique pointer \"%s\"", *this, name);
      Details::serialize_named_option(*this, name, opt);
    }

    // std::shared_ptr

    template <typename T>
    void
    Serializer::serialize(std::string const& name, std::shared_ptr<T>& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize shared pointer to %s \"%s\"",
                       *this, elle::type_info<T>(), name);
      Details::serialize_named_option(*this, name, opt);
    }

    // Raw pointers
    // std::enable_if short-circuits serialization explicit pointer
    // specialization such as BIGNUM*

    template <typename T>
    typename std::enable_if<
      !_details::has_serialize_convert_api<T*, void>(), void>::type
    Serializer::serialize(std::string const& name, T*& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize raw pointer \"%s\"", *this, name);
      Details::serialize_named_option(*this, name, opt);
    }

    /*------.
    | Leafs |
    `------*/

    template <typename T>
    void
    _version_switch(
      Serializer& s,
      T& object,
      std::function<elle::Version ()> version,
      ELLE_SFINAE_IF_WORKS(object.serialize(std::declval<Serializer&>(),
                                            std::declval<Version const&>())))
    {
      object.serialize(s, version());
    }

    template <typename T>
    void
    _version_switch(
      Serializer& s,
      T& object,
      std::function<elle::Version ()> const& version,
      ELLE_SFINAE_OTHERWISE())
    {
      object.serialize(s);
    }

    template <typename T>
    void
    Serializer::serialize_object(std::string const& name,
                                 T& object)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize %s \"%s\"",
                       *this, elle::type_info<T>(), name);
      if (this->_versioned)
      {
        auto version = _details::version_tag<T>(this->versions());
        {
          ELLE_TRACE_SCOPE("%s: serialize version: %s", *this, version);
          this->serialize(".version", version);
        }
        _version_switch(*this, object,
                        [version] { return version; },
                        ELLE_SFINAE_TRY());
      }
      else
        _version_switch(
          *this, object,
          [this] { return _details::version_tag<T>(this->versions()); },
          ELLE_SFINAE_TRY());
    }

    // Special case: don't version versions.
    inline
    void
    Serializer::serialize_object(std::string const& name,
                                 elle::Version& version)
    {
      version.serialize(*this);
    }

    template <typename T>
    void
    Serializer::serialize_pod(std::string const& name,
                              T& v)
    {
      this->_serialize(name, v);
    }

    /*------------.
    | Collections |
    `------------*/

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::string const& name,
                           std::unordered_map<K, V, Rest...>& map)
    {
      _serialize_assoc(name, map);
    }

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::string const& name,
                           std::map<K, V, Rest...>& map)
    {
      _serialize_assoc(name, map);
    }

    template <typename V, typename ... Rest>
    void
    Serializer::_serialize(std::string const& name,
                           std::unordered_map<std::string, V, Rest...>& map)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize umap<str,V> container \"%s\"",
                       *this, name);
      if (this->_out())
      {
        this->_size(map.size());
        for (std::pair<std::string, V> pair: map)
        {
          this->_serialize_dict_key(
          pair.first,
          [&] ()
          {
            this->_serialize_anonymous(pair.first, pair.second);
            elle::SafeFinally leave([&] { this->_leave(name); });
          });
        }
      }
      else
      {
        this->_deserialize_dict_key(
          [&] (std::string const& key)
          {
            V value;
            this->_serialize_anonymous(key, value);
            map.insert({key, value});
          });
      }
    }

    template <typename V, typename ... Rest>
    void
    Serializer::_serialize(std::string const& name,
                           std::unordered_set<V, Rest...>& set)
    {
      this->_serialize_collection(name, set);
    }

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::string const& name,
                           std::unordered_multimap<K, V, Rest...>& map)
    {
      _serialize_assoc(name, map);
    }

    template <typename C>
    void
    Serializer::_serialize_assoc(std::string const& name,
                                 C& map)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize associative container \"%s\"",
                       *this, name);
      typedef typename C::key_type K;
      typedef typename C::mapped_type V;
      if (this->_out())
      {
        this->_serialize_array(
          name,
          map.size(),
          [&] ()
          {
            for (auto const& pair: map)
            {
              if (this->_enter(name))
              {
                elle::SafeFinally leave([&] { this->_leave(name); });
                static_cast<SerializerOut*>(this)->serialize(pair);
              }
            }
          });
      }
      else
      {
        this->_serialize_array(
          name,
          -1,
          [&] ()
          {
            map.emplace(
              static_cast<SerializerIn*>(this)->deserialize<std::pair<K, V>>());
          });
      }
    }

    template <typename As,
              template <typename, typename> class C,
              typename T,
              typename A>
    typename
    std::enable_if<std::is_default_constructible<T>::value, void>::type
    Serializer::serialize(std::string const& name, C<T, A>& collection, as<As>)
    {
      if (this->_enter(name))
      {
        elle::SafeFinally leave([&] { this->_leave(name); });
        this->_serialize(name, collection, as<As>());
      }
    }

    template <typename C>
    typename std::enable_if_exists<
      decltype(
        std::declval<C>().emplace(
          std::declval<elle::serialization::SerializerIn>())),
      void>::type
    Serializer::_deserialize_in_array(std::string const& name,
                                      C& collection)
    {
      collection.emplace(
        Details::deserialize<typename C::value_type>(
          static_cast<SerializerIn&>(*this), 42));
    }

    template <typename C>
    typename std::enable_if_exists<
      decltype(
        std::declval<C>().emplace_back(
          std::declval<elle::serialization::SerializerIn>())),
      void>::type
    Serializer::_deserialize_in_array(std::string const& name,
                                      C& collection)
    {
      collection.emplace_back(
        Details::deserialize<typename C::value_type>(
          static_cast<SerializerIn&>(*this), 42));
    }

    // Specific overload to catch std::vector subclasses (for das, namely).
    template <typename T, typename A>
    void
    Serializer::_serialize(std::string const& name,
                           std::vector<T, A>& collection)
    {
      this->_serialize<std::vector, T, A>(name, collection);
    }

    // Specific overload to catch std::vector subclasses (for das, namely).
    template <typename T, typename C, typename A>
    void
    Serializer::_serialize(std::string const& name,
                           std::set<T, C, A>& collection)
    {
      this->_serialize_collection(name, collection);
    }

    template <typename T, typename I>
    void
    Serializer::_serialize(
      std::string const& name,
      boost::multi_index::multi_index_container<T, I>& collection)
    {
      this->_serialize_collection(name, collection);
    }

    template <typename Repr, typename Ratio>
    void
    Serializer::_serialize(std::string const& name,
                           std::chrono::duration<Repr, Ratio>& duration)
    {
      if (out())
      {
        int64_t count = duration.count();
        int64_t num = Ratio::num;
        int64_t denom = Ratio::den;
        this->_serialize_time_duration(count, num, denom);
      }
      else
      {
        int64_t count;
        int64_t num;
        int64_t denom;
        this->_serialize_time_duration(count, num, denom);
        // FIXME: handle overflows
        count = count * num * Ratio::den / Ratio::num / denom;
        duration = std::chrono::duration<Repr, Ratio>(count);
      }
    }

    template <template <typename, typename> class C, typename T, typename A>
    void
    Serializer::_serialize(std::string const& name,
                           C<T, A>& collection)
    {
      this->_serialize_collection(name, collection);
    }

    template <typename C>
    void
    Serializer::_serialize_collection(std::string const& name,
                                      C& collection)
    {
      if (this->out())
      {
        this->_serialize_array(
          name,
          collection.size(),
          [&] ()
          {
            for (auto& elt: collection)
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("serialize element of \"%s\"", name);
              if (this->_enter(name))
              {
                elle::SafeFinally leave([&] { this->_leave(name); });
                this->_serialize_anonymous
                  (name, const_cast<typename C::value_type&>(elt));
              }
            }
          });
      }
      else
      {
        this->_serialize_array(
          name,
          -1,
          [&] ()
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            ELLE_DEBUG_SCOPE("deserialize element of \"%s\"", name);
            this->_deserialize_in_array(name, collection);
          });
      }
    }

    template <typename As,
              template <typename, typename> class C,
              typename T,
              typename A>
    typename std::enable_if<std::is_default_constructible<T>::value, void>::type
    Serializer::_serialize(std::string const& name,
                           C<T, A>& collection,
                           as<As> as)
    {
      if (this->out())
        static_cast<SerializerOut*>(this)->_serialize(name, collection, as);
      else
      {
        this->_serialize_array(
          name,
          -1,
          [&] ()
          {
            collection.emplace_back(
              static_cast<SerializerIn*>(this)->deserialize<As>());
          });
      }
    }

    template <typename As,
              template <typename, typename> class C,
              typename T,
              typename A>
    void
    SerializerOut::serialize(std::string const& name,
                             C<T, A>& collection,
                             as<As>)
    {
      if (this->_enter(name))
      {
        elle::SafeFinally leave([&] { this->_leave(name); });
        this->_serialize(name, collection, as<As>());
      }
    }

    template <typename As,
              template <typename, typename> class C,
              typename T,
              typename A>
    void
    SerializerOut::_serialize(std::string const& name,
                              C<T, A>& collection,
                              as<As>)
    {
      this->_serialize_array(
        name,
        collection.size(),
        [&] ()
        {
          for (auto& elt: collection)
          {
            if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              As a(elt);
              this->_serialize_anonymous(name, a);
            }
          }
        });
    }

    template <typename T1, typename T2>
    void
    SerializerOut::serialize(std::pair<T1, T2> const& pair)
    {
      this->_serialize_array(
        "",
        2,
        [&] ()
        {
          if (this->_enter(""))
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            ELLE_DEBUG_SCOPE("%s: serialize first member", *this);
            elle::SafeFinally leave([&] { this->_leave(""); });
            this->_serialize_anonymous("", elle::unconst(pair.first));
          }
          if (this->_enter(""))
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            ELLE_DEBUG_SCOPE("%s: serialize second member", *this);
            elle::SafeFinally leave([&] { this->_leave(""); });
            this->_serialize_anonymous("", elle::unconst(pair.second));
          }
        });
    }

    template <typename T1, typename T2>
    void
    Serializer::_serialize(std::string const& name, std::pair<T1, T2>& pair)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize pair \"%s\"", *this, name);
      if (this->_out())
        static_cast<SerializerOut*>(this)->serialize(pair);
      else
        pair =
          static_cast<SerializerIn*>(this)->deserialize<std::pair<T1, T2>>();
    }

    template <typename T>
    void
    Serializer::serialize_forward(T& v)
    {
      this->_serialize_anonymous("SERIALIZE ANONYMOUS", v);
    }

    template <typename S, typename T>
    void
    Serializer::serialize_forward(T& v)
    {
      this->_serialize_anonymous<S>("SERIALIZE ANONYMOUS", v);
    }

    template <typename T>
    void
    Serializer::serialize_context(T& value)
    {
      if (this->in())
        this->_context.get<T>(value);
    }

    template <typename T>
    void
    Serializer::set_context(T value)
    {
      this->_context.set<T>(value);
    }

    template <typename T>
    struct ExceptionMaker
    {
      template <typename U>
      static
      void
      add()
      {}
    };

    template <>
    struct ExceptionMaker<elle::Exception>
    {
      typedef ExceptionMaker<elle::Exception> Self;

      template <typename U>
      static
      void
      add()
      {
        Self::_map()[type_info<U>()] =
          [] (elle::Exception&& e) -> std::exception_ptr
          {
            // FIXME: make_exception cannot use move constructor it seems, which
            // forces all serializable exception to be copyable :(
            return std::make_exception_ptr<U>(static_cast<U&&>(e));
          };
      }

      static
      std::exception_ptr
      make(elle::Exception&& e)
      {
        auto it = Self::_map().find(type_info(e));
        ELLE_ASSERT(it != Self::_map().end());
        return it->second(std::move(e));
      }

      static
      std::map<TypeInfo,
               std::function<std::exception_ptr (elle::Exception&&)> >&
      _map()
      {
        static std::map<
          TypeInfo,
          std::function<std::exception_ptr (elle::Exception&&)> > map;
        return map;
      }
    };

    /*----------.
    | Hierarchy |
    `----------*/

    template <typename T>
    class Hierarchy
    {
    public:
      template <typename U>
      class Register
      {
      public:
        Register(std::string const& name_ = "")
        {
          ELLE_LOG_COMPONENT("elle.serialization");
          auto id = type_info<U>();
          std::string name = name_;
          if (name.empty())
          {
            name = id.name();
            // Normalize stuff between different stdlibs
            while (true)
            {
              auto pos = name.find("std::__1::");
              if (pos == name.npos)
                break;
              name = name.substr(0, pos) + "std::" + name.substr(pos + 10);
            }
          }
          ELLE_TRACE_SCOPE("register dynamic type %s as %s", id, name);
          Hierarchy<T>::_map() [name] =
            [] (SerializerIn& s)
            {
              return elle::make_unique<U>(s.deserialize<U>());
            };
          Hierarchy<T>::_rmap()[id] = name;
          ExceptionMaker<T>::template add<U>();
        }

        void
        poke() const
        {
          ELLE_LOG_COMPONENT("elle.serialization");
          ELLE_DUMP("%s: poke to ensure instantiation", *this);
        }
      };

      typedef
        std::unordered_map<std::string,
                           std::function<std::unique_ptr<T>(SerializerIn&)>>
        TypeMap;
#ifdef INFINIT_WINDOWS
# ifdef ELLE_SERIALIZATION_USE_DLL
  __declspec(dllimport) static TypeMap& _map();
  __declspec(dllimport) static std::map<TypeInfo, std::string>&_rmap();
# else
  __declspec(dllexport) static TypeMap& _map()
  {
    static TypeMap res;
    return res;
  }
  __declspec(dllexport) static std::map<TypeInfo, std::string>&
  _rmap()
  {
    static std::map<TypeInfo, std::string> res;
    return res;
  }
# endif
#else
      static
      TypeMap&
      _map()
      {
        static TypeMap res;
        return res;
      }

      static
      std::map<TypeInfo, std::string>&
      _rmap()
      {
        static std::map<TypeInfo, std::string> res;
        return res;
      }
#endif
    };

    /*--------.
    | Helpers |
    `--------*/

    template <typename T, typename A>
    void
    Serializer::serialize(std::string const& name, T& v, as<A>)
    {
      A actual = A(v);
      this->serialize(name, actual);
      v = T(actual);
    }

    template <typename T> struct is_nullable
    {
      static const bool value = false;
    };
    template <typename T> struct is_nullable<T*> {static const bool value = true;};
    template <typename T> struct is_nullable<std::unique_ptr<T>> {static const bool value = true;};
    template <typename T> struct is_nullable<std::shared_ptr<T>> {static const bool value = true;};

    template <typename T, typename Serializer>
    typename std::enable_if<
      !std::is_base_of<boost::optional_detail::optional_tag, T>::value
      && !is_nullable<T>::value,
      T>::type
    SerializerIn::deserialize(std::string const& name)
    {
      ELLE_ENFORCE(this->_enter(name));
      elle::SafeFinally leave([this, &name] { this->_leave(name); });
      return this->deserialize<T, Serializer>();
    }

    template <typename T, typename Serializer>
    typename std::enable_if<
      std::is_base_of<boost::optional_detail::optional_tag, T>::value
      || is_nullable<T>::value,
      T>::type
    SerializerIn::deserialize(std::string const& name)
    { // We cannot call _enter at this stage for optional types
      T res;
      this->serialize<Serializer>(name, res);
      return res;
    }

    namespace _details
    {
      template <typename T>
      struct serialization_tag
      {
        typedef typename T::serialization_tag type;
      };

      template <typename T>
      struct serialization_tag<std::unique_ptr<T>>
      {
        typedef typename T::serialization_tag type;
      };

      template <typename T>
      struct serialization_tag<T const*>
      {
        typedef typename T::serialization_tag type;
      };

      template <typename T>
      struct serialization_tag<T*>
      {
        typedef typename T::serialization_tag type;
      };

      template <typename T>
      typename std::enable_if_exists<
        decltype(T::dependencies),
        std::unordered_map<elle::TypeInfo, elle::Version>
      >::type
      dependencies(elle::Version const& version, int)
      {
        return T::dependencies.at(version);;
      }

      template <typename T>
      std::unordered_map<elle::TypeInfo, elle::Version>
      dependencies(elle::Version const&, ...)
      {
        return std::unordered_map<elle::TypeInfo, elle::Version>();
      }
    }

    /*--------.
    | Helpers |
    `--------*/

    template <typename T, typename Serializer>
    T
    SerializerIn::deserialize()
    {
      return Details::deserialize<T, Serializer>(*this, 42);
    }

    template <typename ST>
    std::unordered_map<elle::TypeInfo, elle::Version>
    get_serialization_versions(elle::Version const& version)
    {
      auto versions = ST::dependencies.at(version);
      versions.emplace(elle::type_info<ST>(), version);
      return versions;
    }

    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(std::istream& input,
                elle::Version const& version,
                bool versioned,
                boost::optional<Context const&> context = {})
    {
      auto versions = get_serialization_versions
        <typename _details::serialization_tag<T>::type>(version);
      typename Serialization::SerializerIn s(
        input,
        versions,
        versioned);
      if (context)
        s.set_context(context.get());
      return s.template deserialize<T, Serializer>();
    }

    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(std::istream& input, bool version = true,
                boost::optional<Context const&> context = {})
    {
      typename Serialization::SerializerIn s(input, version);
      if (context)
        s.set_context(context.get());
      return s.template deserialize<T, Serializer>();
    }

    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(std::istream& input, std::string const& name,
                bool version = true)
    {
      typename Serialization::SerializerIn s(input, version);
      return s.template deserialize<T, Serializer>(name);
    }

    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(elle::Buffer const& input,
                elle::Version const& version,
                bool versioned = true,
                boost::optional<Context const&> context = {})
    {
      elle::IOStream s(input.istreambuf());
      return deserialize<Serialization, T, Serializer>(
        s, version, versioned, context);
    }

    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(elle::Buffer const& input, bool version = true,
                boost::optional<Context const&> context = {})
    {
      elle::IOStream s(input.istreambuf());
      return deserialize<Serialization, T, Serializer>(s, version, context);
    }

    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(elle::Buffer const& input, std::string const& name,
                bool version = true)
    {
      elle::IOStream s(input.istreambuf());
      return deserialize<Serialization, T, Serializer>(s, name, version);
    }

    // Prevent literal string from being converted to boolean and triggerring
    // the nameless overload.
    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(elle::Buffer const& input, char const* name,
                bool version = true)
    {
      return deserialize<Serialization, T>(input, std::string(name), version);
    }

    template <typename Serialization, typename T>
    void
    serialize(T const& o, std::string const& name,
              std::ostream& output, bool version = true)
    {
      typename Serialization::SerializerOut s(output, version);
      s.serialize(name, o);
    }

    template <typename Serialization, typename T>
    void
    serialize(T const& o, std::ostream& output, bool version = true)
    {
      typename Serialization::SerializerOut s(output, version);
      s.serialize_forward(o);
    }

    template <typename Serializer,
              typename Serialization, typename T, typename ... Args>
    void
    serialize(T const& o,
              std::ostream& output, Args&&... args)
    {
      typename Serialization::SerializerOut s(
        output, std::forward<Args>(args)...);
      s.template serialize_forward<Serializer>(o);
    }

    template <typename Serialization, typename T>
    void
    serialize(std::unique_ptr<T> const& o,
              std::ostream& output,
              elle::Version const& version,
              bool versioned = true)
    {
      typename Serialization::SerializerOut s(
        output, T::serialization_tag::dependencies.at(version), versioned);
      s.serialize_forward(o);
    }

    template <typename Serialization, typename T>
    elle::Buffer
    serialize(T const& o, std::string const& name, bool version = true)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        serialize<Serialization, T>(o, name, s, version);
      }
      return res;
    }

    template <typename Serialization, typename T>
    void
    serialize(T const& o,
              std::ostream& output,
              elle::Version const& version,
              bool versioned = true)
    {
      auto versions =
        _details::dependencies<typename _details::serialization_tag<T>::type>(
          version, 42);
      versions.emplace(
        elle::type_info<typename _details::serialization_tag<T>::type>(),
        version);

      typename Serialization::SerializerOut s(
        output,
        versions,
        versioned);
      s.serialize_forward(o);
    }

    // Prevent literal string from being converted to boolean and triggerring
    // the nameless overload.
    template <typename Serialization, typename T>
    elle::Buffer
    serialize(T const& o, char const* name, bool version = true)
    {
      return serialize<Serialization, T>(o, std::string(name), version);
    }

    template <typename Serialization, typename T>
    elle::Buffer
    serialize(T const& o, bool version = true)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        serialize<Serialization, T>(o, s, version);
      }
      return res;
    }

    template <typename Serialization, typename T>
    elle::Buffer
    serialize(T const& o, elle::Version const& version, bool versioned = true)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        serialize<Serialization, T>(o, s, version, versioned);
      }
      return res;
    }

    /*--------------------------.
    | forward_serialization_tag |
    `--------------------------*/

    template <typename T, bool has>
    struct _forward_serialization_tag
    {
      typedef typename T::serialization_tag serialization_tag;
    };

    template <typename T>
    struct _forward_serialization_tag<T, false>
    {};

    template <typename T>
    struct forward_serialization_tag
      : public _forward_serialization_tag<T, _details::has_version_tag<T>()>
    {};
  }
}

#endif

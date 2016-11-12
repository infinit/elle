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
      ELLE_SERIALIZATION_STATIC_PREDICATE(
        has_serialize_method_api,
        (decltype(std::declval<T>().serialize(std::declval<Serializer&>()))));
      ELLE_SERIALIZATION_STATIC_PREDICATE(
        has_serialize_method_versionned_api,
        (decltype(std::declval<T>().serialize(
                    std::declval<Serializer&>(),
                    std::declval<elle::Version const&>()))));
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
      struct recurse
        : std::false_type
      {};

      template <typename T, typename D>
      struct recurse<std::unique_ptr<T, D>>
        : std::true_type
      {};

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

      template <typename T, typename V>
      void
      assign(T& var, V&& v)
      {
        var = std::move(v);
      }

      template <typename T, typename V>
      void
      assign(boost::optional<T>& var, V&& v)
      {
        if (v)
          var.emplace(std::move(v.get()));
        else
          var.reset();
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

      struct current_name
      {
        inline
        current_name(Serializer& s)
          : _serializer(s)
        {}

        friend
        std::ostream&
        operator <<(std::ostream& s, current_name& c);
        ELLE_ATTRIBUTE(Serializer&, serializer);
      };

      inline
      std::ostream&
      operator <<(std::ostream& s, current_name& c)
      {
        auto name = c._serializer.current_name();
        if (!name.empty())
          s << " \"" << name << "\"";
        return s;
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
          s.serialize_object(*ptr);
        }
        else
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DEBUG_SCOPE("%s: deserialize virtual key%s of type %s",
                           s, _details::current_name(s), type_info<T>());
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
        P& ptr)
      {
        if (s.in())
          _details::_set_ptr(
            ptr,
            new T(Details::deserialize<T>(static_cast<SerializerIn&>(s), 42)));
        else
          Serializer::serialize_switch(s, *ptr);
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

      template <typename T, typename S = void>
      static
      typename std::enable_if<_details::has_serialize_convert_api<T, void>(), T>::type
      deserialize(SerializerIn& self, unsigned)
      {
        // FIXME: convert_api could be factored with the next version if
        // serialize_switch returned the deserialized object
        typename _details::serialization_api<T, S>::Type value;
        Serializer::serialize_switch(self, value);
        return Serialize<T>::convert(value);
      }

      // This overload initializes PODs with "= {}" to avoid warnings.
      template <typename T, typename S = void>
      static
      typename std::enable_if<
        std::is_pod<T>::value &&
        !_details::has_serialize_convert_api<T, void>() &&
        !_details::has_serialize_functions_api<T, void>(),
        T>::type
      deserialize(SerializerIn& self, unsigned)
      {
        T res = {};
        Serializer::serialize_switch<S>(self, res);
        return res;
      }

      template <typename T, typename S = void>
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
        Serializer::serialize_switch<S>(self, res);
        return res;
      }

      // Serialize boost::optionals and smart pointers
      template <typename S, typename T>
      static
      void
      serialize_named_option(Serializer& self, std::string const& name, T& opt);

      enum API
      {
        pod,
        pod_api,
        method,
        method_versionned,
        convert,
        wrapper,
        functions,
        recurse,
      };

      template <typename T, typename S = void>
      static
      API constexpr
      api()
      {
        return
          _details::recurse<T>::value ? recurse :
          _details::has_serialize_convert_api<T, S>() ? convert :
          _details::has_serialize_wrapper_api<T>() ? wrapper :
          _details::has_serialize_functions_api<T, S>() ? functions :
          _details::has_serialize_method_api<T, S>() ? method :
          _details::has_serialize_method_versionned_api<T, S>() ? method_versionned :
          std::is_same<S, void>::value ? pod : pod_api;
      }

      // New

      template <int API, typename T, typename S>
      struct Switch;

      template <typename T, typename S>
      struct Switch<method, T, S>
      {
        static
        void
        value(Serializer& s, T& obj)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: method");
          s.serialize_object(obj);
        }
      };

      template <typename T, typename S>
      struct Switch<method_versionned, T, S>
      {
        static
        void
        value(Serializer& s, T& obj)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: versionned method");
          s.serialize_object(obj);
        }
      };

      template <typename T, typename S>
      struct Switch<pod, T, S>
      {
        static
        void
        value(Serializer& s, T& v)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: POD");
          s._serialize(v);
        }
      };

      template <typename T, typename S>
      struct Switch<pod_api, T, S>
      {
        static
        void
        value(Serializer& s, T& v)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: recurse POD");
          s._serialize<S>(v);
        }
      };

      template <typename T, typename S>
      struct Switch<convert, T, S>
      {
        static
        void
        value(Serializer& s, T& v)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: convert");
          using API = typename _details::serialization_api<T, S>;
          using type = typename API::Type;
          if (s.out())
          {
            type value(Serialize<T>::convert(v));
            Serializer::serialize_switch<S>(s, value);
          }
          else
          {
            type value;
            Serializer::serialize_switch<S>(s, value);
            v = Serialize<T>::convert(value);
          }
        }
      };

      template <typename T, typename S>
      struct Switch<wrapper, T, S>
      {
        static
        void
        value(Serializer& s, T& v)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: wrapper");
          typedef typename Serialize<T>::Wrapper Wrapper;
          Wrapper wrapper(v);
          Serializer::serialize_switch<Wrapper>(s, wrapper);
        }
      };

      template <typename T, typename S>
      struct Switch<functions, T, S>
      {
        static
        void
        value(Serializer& s, T& v)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: functions");
          if (s.out())
            _details::serialization_api<T, S>::serialize(
              v, static_cast<SerializerOut&>(s));
          else
            _details::assign(
              v, _details::serialization_api<T, S>::deserialize(
                static_cast<SerializerIn&>(s)));
        }

        static
        void
        value(SerializerOut& s, T& v)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: functions");
          _details::serialization_api<T, S>::serialize(v, s);
        }
      };

      template <typename T, typename S>
      struct Switch<recurse, T, S>
      {
        template <typename P, typename D>
        static
        void
        value(Serializer& s, std::unique_ptr<P, D>& v)
        {
          ELLE_LOG_COMPONENT("elle.serialization.Serializer");
          ELLE_DUMP("API: recurse std::unique_ptr");
          if (s.out())
          {
            P* p = v.get();
            Serializer::serialize_switch<S>(s, p);
          }
          else
          {
            P* p = nullptr;
            Serializer::serialize_switch<S>(s, p);
            v.reset(p);
          }
        }
      };
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
          bool(o),
          [&]
          {
            Serializer::serialize_switch(s, elle::unconst(*o));
          });
      }

      static
        boost::optional<T>
        deserialize(elle::serialization::SerializerIn& s)
      {
        boost::optional<T> res;
        s._serialize_option(
          false,
          [&]
          {
            res.emplace(Serializer::Details::deserialize<T>(
                          static_cast<SerializerIn&>(s), 42));
          });
        return res;
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
          bool(ptr),
          [&]
          {
            Serializer::Details::_smart_virtual_switch<std::shared_ptr<T>, T>
              (s, elle::unconst(ptr));
          });
      }

      static
      std::shared_ptr<T>
      deserialize(elle::serialization::SerializerIn& s)
      {
        std::shared_ptr<T> ptr;
        s._serialize_option(
          true,
          [&]
          {
            Serializer::Details::_smart_virtual_switch<std::shared_ptr<T>, T>
              (s, ptr);
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
          bool(ptr),
          [&]
          {
            Serializer::Details::_smart_virtual_switch<T*, T>(s, ptr);
          });
      }

      static
      T*
      deserialize(elle::serialization::SerializerIn& s)
      {
        T* ptr = nullptr;
        s._serialize_option(
          true,
          [&]
          {
            Serializer::Details::_smart_virtual_switch<T*, T>(s, ptr);
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
        s._serialize_anonymous_exception(e);
      }

      static
      std::exception_ptr
      deserialize(elle::serialization::SerializerIn& s)
      {
        std::exception_ptr e;
        s._serialize_anonymous_exception(e);
        return e;
      }
    };

    template <typename S, typename Ser, typename T>
    void
    Serializer::serialize_switch(Ser& s, T& v)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      if (s.out())
      {
        ELLE_DUMP("value: %s", v);
      }
      static_assert(
        !std::is_base_of<VirtuallySerializableBase, T>::value,
        "serialize VirtuallySerializable objects through a pointer type");
      Details::template Switch<Details::api<T, S>(), T, S>::value(s, v);
      if (s.in())
      {
        ELLE_DUMP("value: %s", v);
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
      {
        ELLE_LOG_COMPONENT("elle.serialization.Serializer");
        ELLE_DUMP("type: %s", elle::type_info<T>());
        Serializer::serialize_switch<S>(*this, v);
      }
    }

    /*------------------------------------.
    | Serialization: Options and pointers |
    `------------------------------------*/

    template <typename S, typename T>
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
          Serializer::serialize_switch<S>(self, opt);
          filled = true;
        });
      if (!filled)
      {
        ELLE_DEBUG("reset option");
        _details::option_reset(opt);
      }
    }

    // boost::optional

    template <typename S, typename T>
    void
    Serializer::serialize(std::string const& name, boost::optional<T>& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize option \"%s\"", *this, name);
      Details::serialize_named_option<S>(*this, name, opt);
    }

    // std::unique_ptr

    template <typename S, typename T, typename D>
    void
    Serializer::serialize(std::string const& name,
                          std::unique_ptr<T, D>& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize unique pointer \"%s\"", *this, name);
      Details::serialize_named_option<S>(*this, name, opt);
    }

    // std::shared_ptr

    template <typename S, typename T>
    void
    Serializer::serialize(std::string const& name, std::shared_ptr<T>& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize shared pointer to %s \"%s\"",
                       *this, elle::type_info<T>(), name);
      Details::serialize_named_option<S>(*this, name, opt);
    }

    // Raw pointers
    // std::enable_if short-circuits serialization explicit pointer
    // specialization such as BIGNUM*

    template <typename S, typename T>
    typename std::enable_if<
      !_details::has_serialize_convert_api<T*, void>(), void>::type
    Serializer::serialize(std::string const& name, T*& opt)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize raw pointer \"%s\"", *this, name);
      Details::serialize_named_option<S>(*this, name, opt);
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
    Serializer::serialize_object(T& object)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize %s%s",
                       *this, elle::type_info<T>(),
                       _details::current_name(*this));
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
    Serializer::serialize_object(elle::Version& version)
    {
      version.serialize(*this);
    }

    /*------------.
    | Collections |
    `------------*/

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::unordered_map<K, V, Rest...>& map)
    {
      _serialize_assoc(map);
    }

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::map<K, V, Rest...>& map)
    {
      _serialize_assoc(map);
    }

    template <typename V, typename ... Rest>
    void
    Serializer::_serialize(std::unordered_map<std::string, V, Rest...>& map)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize dictionary%s",
                       this, _details::current_name(*this));
      if (this->_out())
      {
        this->_size(map.size());
        for (std::pair<std::string, V> pair: map)
        {
          this->_serialize_dict_key(
            pair.first,
            [&] ()
            {
              Serializer::serialize_switch(*this, pair.second);
              // FIXME: wtf is this leave ?
              elle::SafeFinally leave(
                [&] { this->_leave(this->current_name()); });
            });
        }
      }
      else
      {
        this->_deserialize_dict_key(
          [&] (std::string const& key)
          {
            V value;
            Serializer::serialize_switch(*this, value);
            map.insert({key, value});
          });
      }
    }

    template <typename V, typename ... Rest>
    void
    Serializer::_serialize(std::unordered_set<V, Rest...>& set)
    {
      this->_serialize_collection(set);
    }

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::unordered_multimap<K, V, Rest...>& map)
    {
      _serialize_assoc(map);
    }

    template <typename C>
    void
    Serializer::_serialize_assoc(C& map)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize associative container%s",
                       this, _details::current_name(*this));
      typedef typename C::key_type K;
      typedef typename C::mapped_type V;
      if (this->_out())
      {
        this->_serialize_array(
          map.size(),
          [&] ()
          {
            for (auto const& pair: map)
            {
              if (auto entry = this->enter(this->current_name()))
                static_cast<SerializerOut*>(this)->serialize(pair);
            }
          });
      }
      else
      {
        this->_serialize_array(
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

    template <typename S, typename C>
    typename std::enable_if_exists<
      decltype(
        std::declval<C>().emplace(
          std::declval<elle::serialization::SerializerIn>())),
      void>::type
    Serializer::_deserialize_in_array(C& collection)
    {
      collection.emplace(
        Details::deserialize<typename C::value_type, S>(
          static_cast<SerializerIn&>(*this), 42));
    }

    template <typename S, typename C>
    typename std::enable_if_exists<
      decltype(
        std::declval<C>().emplace_back(
          std::declval<elle::serialization::SerializerIn>())),
      void>::type
    Serializer::_deserialize_in_array(C& collection)
    {
      collection.emplace_back(
        Details::deserialize<typename C::value_type, S>(
          static_cast<SerializerIn&>(*this), 42));
    }

    // Specific overload to catch std::vector subclasses (for das, namely).
    template <typename S, typename T, typename A>
    void
    Serializer::_serialize(std::vector<T, A>& collection)
    {
      this->_serialize<S, std::vector, T, A>(collection);
    }

    // Specific overload to catch std::vector subclasses (for das, namely).
    template <typename T, typename C, typename A>
    void
    Serializer::_serialize(std::set<T, C, A>& collection)
    {
      this->_serialize_collection(collection);
    }

    template <typename T, typename I>
    void
    Serializer::_serialize(
      boost::multi_index::multi_index_container<T, I>& collection)
    {
      this->_serialize_collection(collection);
    }

    template <typename Repr, typename Ratio>
    void
    Serializer::_serialize(std::chrono::duration<Repr, Ratio>& duration)
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

    template <typename S,
              template <typename, typename> class C,
              typename T,
              typename A>
    void
    Serializer::_serialize(C<T, A>& collection)
    {
      this->_serialize_collection<S>(collection);
    }

    template <typename S, typename C>
    void
    Serializer::_serialize_collection(C& collection)
    {
      if (this->out())
      {
        this->_serialize_array(
          collection.size(),
          [&] ()
          {
            for (auto& elt: collection)
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("serialize element of \"%s\"",
                               this->current_name());
              if (auto entry = this->enter(this->current_name()))
                Serializer::serialize_switch<S>
                  (static_cast<SerializerOut&>(*this),
                   const_cast<typename C::value_type&>(elt));
            }
          });
      }
      else
      {
        this->_serialize_array(
          -1,
          [&] ()
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            ELLE_DEBUG_SCOPE("deserialize element of \"%s\"",
                             this->current_name());
            this->_deserialize_in_array<S>(collection);
          });
      }
    }

    template <typename As,
              template <typename, typename> class C,
              typename T,
              typename A>
    typename std::enable_if<std::is_default_constructible<T>::value, void>::type
    Serializer::_serialize(C<T, A>& collection,
                           as<As> as)
    {
      if (this->out())
        static_cast<SerializerOut*>(this)->_serialize(collection, as);
      else
      {
        this->_serialize_array(
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
      if (auto entry = this->enter(name))
        this->_serialize(name, collection, as<As>());
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
        collection.size(),
        [&] ()
        {
          for (auto& elt: collection)
          {
            if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              As a(elt);
              Serializer::serialize_switch(*this, a);
            }
          }
        });
    }

    template <typename T1, typename T2>
    void
    SerializerOut::serialize(std::pair<T1, T2> const& pair)
    {
      this->_serialize_array(
        2,
        [&] ()
        {
          if (this->_enter(""))
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            ELLE_DEBUG_SCOPE("%s: serialize first member", *this);
            elle::SafeFinally leave([&] { this->_leave(""); });
            Serializer::serialize_switch(*this, elle::unconst(pair.first));
          }
          if (this->_enter(""))
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            ELLE_DEBUG_SCOPE("%s: serialize second member", *this);
            elle::SafeFinally leave([&] { this->_leave(""); });
            Serializer::serialize_switch(*this, elle::unconst(pair.second));
          }
        });
    }

    template <typename T1, typename T2>
    void
    Serializer::_serialize(std::pair<T1, T2>& pair)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize pair%s",
                       this, _details::current_name(*this));
      if (this->_out())
        static_cast<SerializerOut*>(this)->serialize(pair);
      else
        pair =
          static_cast<SerializerIn*>(this)->deserialize<std::pair<T1, T2>>();
    }

    template <typename S, typename T>
    void
    Serializer::serialize_forward(T& v)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize %s", this, elle::type_info<T>());
      Serializer::serialize_switch<S>(*this, v);
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

    template <typename S, typename T, typename A>
    void
    Serializer::serialize(std::string const& name, T& v, as<A>)
    {
      A actual = A(v);
      this->serialize<S>(name, actual);
      v = T(actual);
    }

    template <typename T> struct is_nullable
    {
      static const bool value = false;
    };
    template <typename T> struct is_nullable<T*> {static const bool value = true;};
    template <typename T> struct is_nullable<std::unique_ptr<T>> {static const bool value = true;};
    template <typename T> struct is_nullable<std::shared_ptr<T>> {static const bool value = true;};

    template <typename T, typename S>
    typename std::enable_if<
      !std::is_base_of<boost::optional_detail::optional_tag, T>::value
      && !is_nullable<T>::value,
      T>::type
    SerializerIn::deserialize(std::string const& name)
    {
      auto entry = this->enter(name);
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_DUMP("type: %s", elle::type_info<T>());
      ELLE_ENFORCE(entry);
      return this->deserialize<T, S>();
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

    template <typename T, typename S>
    T
    SerializerIn::deserialize()
    {
      return Details::deserialize<T, S>(*this, 42);
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
    deserialize(std::istream& input,
                std::string const& name,
                bool version = true)
    {
      typename Serialization::SerializerIn s(input, version);
      return s.template deserialize<T, Serializer>(name);
    }

    // Prevent literal string from being converted to boolean and triggerring
    // the nameless overload.
    template <typename Serialization, typename T, typename Serializer = void>
    T
    deserialize(std::istream& input,
                char const* name,
                bool version = true)
    {
      return deserialize<Serialization, T, Serializer>(
        input, std::string(name), version);
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

    // Stream, named
    template <typename Serializer,
              typename Serialization,
              typename T,
              typename ... Args>
    void
    serialize(T const& o,
              std::string const& name,
              std::ostream& output,
              Args&&... args)
    {
      typename Serialization::SerializerOut s(
        output, std::forward<Args>(args)...);
      s.template serialize<Serializer>(name, o);
    }

    template <typename T, typename ... Args>
    struct FirstArgIsNot
    {};

    template <typename T>
    struct FirstArgIsNot<T>
    {
      static bool constexpr value = true;
    };

    template <typename T, typename First, typename ... Args>
    struct FirstArgIsNot<T, First, Args ...>
    {
      static bool constexpr value = !std::is_base_of<
        T,
        typename std::remove_const<
          typename std::remove_reference<First>::type>::type>::value;
    };

    // Stream, anonymous
    template <typename Serializer,
              typename Serialization,
              typename T,
              typename ... Args>
    std::enable_if_t<FirstArgIsNot<elle::Version, Args...>::value, void>
    serialize(T const& o,
              std::ostream& output,
              Args&&... args)
    {
      typename Serialization::SerializerOut s(
        output, std::forward<Args>(args)...);
      s.template serialize_forward<Serializer>(o);
    }

    // Stream, anonymous, Version
    template <typename Serializer,
              typename Serialization,
              typename T,
              typename ... Args>
    void
    serialize(std::unique_ptr<T> const& o,
              std::ostream& output,
              elle::Version const& version,
              Args&& ... args)
    {
      typename Serialization::SerializerOut s(
        output,
        T::serialization_tag::dependencies.at(version),
        std::forward<Args>(args)...);
      s.template serialize_forward<Serializer>(o);
    }

    // Stream, anonymous, Version
    template <typename Serializer,
              typename Serialization,
              typename T,
              typename ... Args>
    void
    serialize(T const& o,
              std::ostream& output,
              elle::Version const& version,
              Args&& ... args)
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
        std::forward<Args>(args)...);
      s.template serialize_forward<Serializer>(o);
    }

    // Buffer, named
    template <typename Serializer,
              typename Serialization,
              typename T,
              typename ... Args>
    std::enable_if_t<FirstArgIsNot<std::ostream, Args...>::value, elle::Buffer>
    serialize(T const& o,
              std::string const& name,
              Args&& ... args)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        serialize<Serializer, Serialization, T>(
          o, name, s, std::forward<Args>(args)...);
      }
      return res;
    }

    // Prevent literal string from being converted to boolean and triggerring
    // the nameless overload.
    template <typename Serializer,
              typename Serialization,
              typename T,
              typename ... Args>
    std::enable_if_t<FirstArgIsNot<std::ostream, Args...>::value, elle::Buffer>
    serialize(T const& o,
              char const* name,
              Args&& ... args)
    {
      return serialize<Serializer, Serialization, T>(
        o, std::string(name), std::forward<Args>(args)...);
    }

    // Buffer, anonymous
    template <typename Serializer,
              typename Serialization,
              typename T,
              typename ... Args>
    std::enable_if_t<FirstArgIsNot<std::ostream, Args...>::value, elle::Buffer>
    serialize(T const& o,
              Args&& ... args)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        serialize<Serializer, Serialization, T>(
          o, s, std::forward<Args>(args)...);
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

# include <elle/serialization/SerializerOut.hxx>

#endif

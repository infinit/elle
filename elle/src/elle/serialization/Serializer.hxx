#ifndef ELLE_SERIALIZATION_SERIALIZER_HXX
# define ELLE_SERIALIZATION_SERIALIZER_HXX

# include <boost/optional.hpp>

# include <elle/Backtrace.hh>
# include <elle/TypeInfo.hh>
# include <elle/finally.hh>
# include <elle/serialization/Error.hh>
# include <elle/serialization/SerializerIn.hh>
# include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    namespace
    {
      template <typename T>
      typename std::enable_if<std::is_base_of<VirtuallySerializable, T>::value, void>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& obj,
        ELLE_SFINAE_IF_WORKS(obj.serialize(ELLE_SFINAE_INSTANCE(Serializer))))
      {
        if (s.out())
        {
          auto id = type_info(obj);
          auto const& map = Hierarchy<T>::_rmap();
          auto it = map.find(id);
          if (it == map.end())
          {
            ELLE_LOG_COMPONENT("elle.serialization.Serializer");
            auto message =
              elle::sprintf("unable to get serialization name for type %s", id);
            ELLE_WARN("%s", message);
            throw Error(message);
          }
          auto type_name = it->second;
          s.serialize(T::virtually_serializable_key, type_name);
        }
        s.serialize_object(name, obj);
      }

      template <typename T>
      typename std::enable_if<!std::is_base_of<VirtuallySerializable, T>::value, void>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& obj,
        ELLE_SFINAE_IF_WORKS(obj.serialize(ELLE_SFINAE_INSTANCE(Serializer))))
      {
        s.serialize_object(name, obj);
      }

      template <typename T>
      typename std::enable_if<!std::is_base_of<VirtuallySerializable, T>::value, void>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& obj,
        ELLE_SFINAE_IF_WORKS(
          obj.serialize(ELLE_SFINAE_INSTANCE(Serializer),
                        ELLE_SFINAE_INSTANCE(elle::Version))))
      {
        s.serialize_object(name, obj);
      }

      template <typename T>
      void
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& v,
        ELLE_SFINAE_OTHERWISE())
      {
        s.serialize_pod(name, v);
      }

      template <typename T>
      typename std::conditional<true, void, typename Serialize<T>::Type>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& v,
        ELLE_SFINAE_IF_POSSIBLE())
      {
        typedef typename Serialize<T>::Type Type;
        if (s.out())
        {
          Type value(Serialize<T>::convert(v));
          _serialize_switch<Type>(s, name, value, ELLE_SFINAE_TRY());
        }
        else
        {
          Type value;
          _serialize_switch<Type>(s, name, value, ELLE_SFINAE_TRY());
          v = Serialize<T>::convert(value);
        }
      }

      template <typename T>
      typename std::conditional<true, void, typename Serialize<T>::Wrapper>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& v,
        ELLE_SFINAE_IF_POSSIBLE())
      {
        typedef typename Serialize<T>::Wrapper Wrapper;
        Wrapper wrapper(v);
        _serialize_switch<Wrapper>(s, name, wrapper, ELLE_SFINAE_TRY());
      }

      template <typename T>
      constexpr
      typename std::enable_if<
        !std::is_base_of<boost::optional_detail::optional_tag, T>::value &&
        std::is_constructible<T, SerializerIn&>::value,
        bool>::type
      _is_unserializable_inplace(int)
      {
        return true;
      };

      template <typename T>
      constexpr
      bool
      _is_unserializable_inplace(unsigned int)
      {
        return false;
      };

      template <typename T>
      constexpr
      bool
      is_unserializable_inplace()
      {
        return _is_unserializable_inplace<T>(42);
      };

      template <typename T>
      typename std::enable_if<!is_unserializable_inplace<T>(), T>::type
      _deserialize(elle::serialization::SerializerIn& input)
      {
        T res;
        input.serialize_forward(res);
        return res;
      }

      template <typename T>
      typename std::enable_if<is_unserializable_inplace<T>(), T>::type
      _deserialize(elle::serialization::SerializerIn& input)
      {
        static_assert(is_unserializable_inplace<T>(), "");
        return T(input);
      }

    }

    template <typename T>
    void
    Serializer::serialize(std::string const& name, T& v)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize %s", *this, name);
      if (this->_enter(name))
      {
        elle::SafeFinally leave([&] { this->_leave(name); });
        this->_serialize_anonymous(name, v);
      }
    }

    template <typename T>
    typename std::enable_if<is_serializer_constructible<T>(), void>::type
    Serializer::_deserialize_in_option(std::string const& name,
                                       boost::optional<T>& opt)
    {
      ELLE_ENFORCE(this->_enter(name));
      elle::SafeFinally leave([&] { this->_leave(name); });
      opt.emplace(static_cast<SerializerIn&>(*this));
    }

    template <typename T>
    typename std::enable_if<!is_serializer_constructible<T>(), void>::type
    Serializer::_deserialize_in_option(std::string const& name,
                                       boost::optional<T>& opt)
    {
      T value;
      this->serialize(name, value);
      opt = std::move(value);
    }

    template <typename T>
    void
    Serializer::serialize(std::string const& name, boost::optional<T>& opt)
    {
      if (this->_out())
        this->_serialize_option(name,
                                bool(opt),
                                [&]
                                {
                                  this->serialize(name, opt.get());
                                });
      else
        this->_serialize_option(name,
                                bool(opt),
                                [&]
                                {
                                  this->_deserialize_in_option(name, opt);
                                });
    }

    class Serializer::Details
    {
    public:
      template <typename P, typename T>
      static
      typename std::enable_if<
        is_unserializable_inplace<T>(),
        void
      >::type
      _smart_emplace_switch(
        Serializer& s,
        std::string const& name,
        P& ptr)
      {
        ptr.reset(new T(static_cast<SerializerIn&>(s)));
      }

      template <typename P, typename T>
      static
      typename std::enable_if<
        !is_unserializable_inplace<T>(),
        void
      >::type
      _smart_emplace_switch(
        Serializer& s,
        std::string const& name,
        P& ptr)
      {
        ptr.reset(new T);
        s._serialize_anonymous(name, *ptr);
      }

      template <typename P, typename T>
      static
      typename std::enable_if<
        std::is_base_of<VirtuallySerializable, T>::value,
        void
      >::type
      _smart_virtual_switch(
        Serializer& s,
        std::string const& name,
        P& ptr)
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
          throw Error(elle::sprintf("unable to deserialize type %s",
                                    type_name));
        ptr.reset(it->second(static_cast<SerializerIn&>(s)).release());
      }

      template <typename P, typename T>
      static
      typename std::enable_if<
        !std::is_base_of<VirtuallySerializable, T>::value,
        void
      >::type
      _smart_virtual_switch(
        Serializer& s,
        std::string const& name,
        P& ptr)
      {
        _smart_emplace_switch<P, T>(s, name, ptr);
      }
    };

    template <typename T>
    void
    Serializer::serialize(std::string const& name, std::unique_ptr<T>& opt)
    {
      if (this->_out())
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            this->serialize(name, *opt);
          });
      else
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              Details::_smart_virtual_switch<std::unique_ptr<T>, T>
                (*this, name, opt);
            }
          });
    }

    // FIXME: duplicated with ::serialize(name, unique_ptr)
    template <typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name,
                                     std::unique_ptr<T>& opt)
    {
      if (this->_out())
      {
        ELLE_ASSERT(opt.get());
        this->_serialize_anonymous(name, *opt);
      }
      else
      {
        Details::_smart_virtual_switch<std::unique_ptr<T>, T>
          (*this, name, opt);
      }
    }

    // FIXME: duplicated
    template <typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name,
                                     boost::optional<T>& opt)
    {
      if (this->_out())
      {
        ELLE_ASSERT(bool(opt));
        this->_serialize_anonymous(name, *opt);
      }
      else
        if (static_cast<SerializerIn&>(*this)._option_filled())
          this->_deserialize_in_option(name, opt);
        else
          opt.reset();
    }

    template <typename T>
    void
    Serializer::serialize(std::string const& name, std::shared_ptr<T>& opt)
    {
      if (this->_out())
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            this->serialize(name, *opt);
          });
      else
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              Details::_smart_virtual_switch<std::shared_ptr<T>, T>
                (*this, name, opt);
            }
          });
    }

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
                           std::unordered_multimap<K, V, Rest...>& map)
    {
      _serialize_assoc(name, map);
    }

    template <typename C>
    void
    Serializer::_serialize_assoc(std::string const& name,
                           C& map)
    {
      typedef typename C::key_type K;
      typedef typename C::mapped_type V;
      if (this->_out())
      {
        this->_serialize_array(
          name,
          map.size(),
          [&] ()
          {
            for (std::pair<K, V> pair: map)
            {
              if (this->_enter(name))
              {
                elle::SafeFinally leave([&] { this->_leave(name); });
                this->_serialize_anonymous(name, pair);
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
            // FIXME: Use emplace if possible.
            std::pair<K, V> p;
            this->_serialize_anonymous(name, p);
            map.insert(p);
          });
      }

    }

    template <typename T, typename A>
    void
    Serializer::serialize(std::string const& name, T& v, as<A>)
    {
      A actual = A(v);
      this->serialize(name, actual);
      v = T(actual);
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

    template <typename T>
    void
    _version_switch(
      Serializer& s,
      T& object,
      elle::Version version,
      ELLE_SFINAE_IF_WORKS(object.serialize(ELLE_SFINAE_INSTANCE(Serializer),
                                            elle::Version())))
    {
      object.serialize(s, version);
    }

    template <typename T>
    void
    _version_switch(
      Serializer& s,
      T& object,
      elle::Version const& version,
      ELLE_SFINAE_OTHERWISE())
    {
      object.serialize(s);
    }

    template <typename T, typename U>
    struct ScopedAssignment
    {
      ScopedAssignment(T& var, U value)
        : _var(&var)
        , _previous(std::move(var))
      {
        var = std::move(value);
      }

      ScopedAssignment(ScopedAssignment&& assignment)
        : _var(assignment._var)
        , _previous(std::move(assignment._previous))
      {
        assignment._var = nullptr;
      }

      ~ScopedAssignment()
      {
        if (this->_var)
          try
          {
            *this->_var = std::move(this->_previous);
          }
          catch (...)
          {
            ELLE_ABORT("exception in scoped assignment");
          }
      }

      ELLE_ATTRIBUTE(T*, var);
      ELLE_ATTRIBUTE(T, previous);
    };

    template <typename T, typename U>
    inline
    ScopedAssignment<T, U>
    scoped_assignment(T& var, U value)
    {
      return ScopedAssignment<T, U>(var, std::move(value));
    }

    template <typename T>
    void
    Serializer::serialize_object(std::string const& name,
                                 T& object)
    {
      if (this->_versioned)
      {
        ELLE_LOG_COMPONENT("elle.serialization.Serializer");
        elle::Version v(this->_version);
        {
          ELLE_TRACE_SCOPE("%s: serialize version: %s", *this, this->_version);
          auto guard = scoped_assignment(this->_versioned, false);
          this->serialize(".version", v);
        }
        _version_switch(*this, object, std::move(v), ELLE_SFINAE_TRY());
      }
      else
        _version_switch(*this, object, elle::Version(), ELLE_SFINAE_TRY());
    }

    template <typename T>
    void
    Serializer::serialize_pod(std::string const& name,
                              T& v)
    {
      this->_serialize(name, v);
    }

    template <typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name, T& v)
    {
      _serialize_switch(*this, name, v, ELLE_SFINAE_TRY());
    }

    template <template <typename, typename> class C, typename T, typename A>
    typename std::enable_if<is_serializer_constructible<T>(), void>::type
    Serializer::_deserialize_in_array(std::string const& name,
                                      C<T, A>& collection)
    {
      collection.emplace_back(static_cast<SerializerIn&>(*this));
    }

    template <template <typename, typename> class C, typename T, typename A>
    typename std::enable_if<!is_serializer_constructible<T>(), void>::type
    Serializer::_deserialize_in_array(std::string const& name,
                                      C<T, A>& collection)
    {
      collection.emplace_back();
      this->_serialize_anonymous(name, collection.back());
    }

    template <template <typename, typename> class C, typename T, typename A>
    void
    Serializer::_serialize(std::string const& name,
                           C<T, A>& collection)
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
              if (this->_enter(name))
              {
                elle::SafeFinally leave([&] { this->_leave(name); });
                this->_serialize_anonymous(name, elt);
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
            this->_deserialize_in_array<C, T, A>(name, collection);
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
            collection.emplace_back();
            As a(collection.back());
            this->_serialize_anonymous(name, a);
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
    Serializer::_serialize(std::string const& name, std::pair<T1, T2>& pair)
    {
      if (this->_out())
      {
        this->_serialize_array(
          name,
          2,
          [&] ()
          {
            if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              this->_serialize_anonymous(name, pair.first);
            }
            if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              this->_serialize_anonymous(name, pair.second);
            }
          });
      }
      else
      {
        int i = 0;
        this->_serialize_array(
          name,
          -1,
          [&] ()
          {
            if (i == 0)
              this->_serialize_anonymous(name, pair.first);
            else if (i == 1)
              this->_serialize_anonymous(name, pair.second);
            else
              throw Error("too many values to unpack for a pair");
            ++i;
          });
      }
    }

    template <typename T>
    void
    Serializer::serialize_forward(T& v)
    {
      this->_serialize_anonymous("FIXME BUT I DON'T THINK THIS IS USED", v);
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
          [] (elle::Exception const& e) -> std::exception_ptr
          {
            return std::make_exception_ptr<U>(static_cast<U const&>(e));
          };
      }

      static
      std::exception_ptr
      make(elle::Exception& e)
      {
        auto it = Self::_map().find(type_info(e));
        ELLE_ASSERT(it != Self::_map().end());
        return it->second(e);
      }

      static
      std::map<TypeInfo,
               std::function<std::exception_ptr (elle::Exception const&)> >&
      _map()
      {
        static std::map<
          TypeInfo,
          std::function<std::exception_ptr (elle::Exception const&)> > map;
        return map;
      }
    };

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
          if (name_.empty())
            ELLE_TRACE_SCOPE("register dynamic type %s", id);
          else
            ELLE_TRACE_SCOPE("register dynamic type %s as %s", id, name_);
          std::string name = name_.empty() ? id.name() : name_;
          Hierarchy<T>::_map() [name] =
            [] (SerializerIn& s) { return elle::make_unique<U>(s); };
          Hierarchy<T>::_rmap()[id] = name;
          ExceptionMaker<T>::template add<U>();
        }
      };

      static
      std::unordered_map<std::string,
                         std::function<std::unique_ptr<T>(SerializerIn&)>>&
      _map()
      {
        static std::unordered_map<std::string, std::function<std::unique_ptr<T>(SerializerIn&)>> res;
        return res;
      }

      static
      std::map<TypeInfo, std::string>&
      _rmap()
      {
        static std::map<TypeInfo, std::string> res;
        return res;
      }
    };

    template <typename T>
    T
    SerializerIn::deserialize(std::string const& name)
    {
      ELLE_ENFORCE(this->_enter(name));
      elle::SafeFinally leave([this, &name] { this->_leave(name); });
      return this->deserialize<T>();
    }

    template <typename T>
    T
    SerializerIn::deserialize()
    {
      return _deserialize<T>(*this);
    }

    template <typename T, typename Serialization>
    T
    deserialize(std::istream& input)
    {
      typename Serialization::SerializerIn s(input);
      return s.template deserialize<T>();
    }

    template <typename T, typename Serialization>
    T
    deserialize(std::istream& input, std::string const& name)
    {
      typename Serialization::SerializerIn s(input);
      return s.template deserialize<T>(name);
    }

    template <typename T, typename Serialization>
    T
    deserialize(elle::Buffer const& input)
    {
      elle::IOStream s(input.istreambuf());
      return deserialize<T, Serialization>(s);
    }

    template <typename T, typename Serialization>
    T
    deserialize(elle::Buffer const& input, std::string const& name)
    {
      elle::IOStream s(input.istreambuf());
      return deserialize<T, Serialization>(s, name);
    }

    template <typename T, typename Serialization>
    elle::Buffer
    serialize(T const& o, std::string const& name)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        typename Serialization::SerializerOut output(s);
        output.serialize(name, o);
      }
      return res;
    }

    template <typename T, typename Serialization>
    elle::Buffer
    serialize(T const& o)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        typename Serialization::SerializerOut output(s);
        output.serialize_forward(o);
      }
      return res;
    }
  }
}

#endif

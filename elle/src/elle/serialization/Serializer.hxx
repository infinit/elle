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

namespace elle
{
  namespace serialization
  {
    template <typename T>
    typename std::enable_if_exists<decltype(T::serialization_tag::version),
                                   elle::Version>::type
    _version_tag(int)
    {
      return T::serialization_tag::version;
    }

    template <typename T>
    elle::Version
    _version_tag(...)
    {
      throw elle::Error(elle::sprintf("no serialization version tag for %s",
                                      elle::type_info<T>().name()));
    }

    namespace
    {
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
        s.serialize_with_struct_serialize(name, v, true);
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
        ELLE_LOG_COMPONENT("elle.serialization.Serializer");
        static_assert(is_unserializable_inplace<T>(), "");
        // FIXME: factor reading version
        // FIXME: use that version
        if (input.versioned())
        {
          auto version = _version_tag<T>(42);
          {
            ELLE_TRACE_SCOPE("serialize version: %s", version);
            input.serialize(".version", version);
          }
        }
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
    typename std::enable_if<is_unserializable_inplace<T>(), void>::type
    Serializer::_deserialize_in_option(std::string const& name,
                                       boost::optional<T>& opt)
    {
      opt.emplace(static_cast<SerializerIn&>(*this));
    }

    template <typename T>
    typename std::enable_if<!is_unserializable_inplace<T>(), void>::type
    Serializer::_deserialize_in_option(std::string const& name,
                                       boost::optional<T>& opt)
    {
      T value;
      this->_serialize_anonymous(name, value);
      opt = std::move(value);
    }

    template<typename T>
    void
    Serializer::serialize_with_struct_serialize(
      std::string const& name,
      T& v,
      bool anonymous)
    {
      typedef typename Serialize<T>::Type Type;
      if (anonymous)
      {
        if (this->out())
        {
          Type value(Serialize<T>::convert(v));
          _serialize_switch<Type>(*this, name, value, ELLE_SFINAE_TRY());
        }
        else
        {
          Type value;
          _serialize_switch<Type>(*this, name, value, ELLE_SFINAE_TRY());
          v = Serialize<T>::convert(value);
        }
      }
      else
      {
        if (this->_enter(name))
        {
          elle::SafeFinally leave([&] { this->_leave(name); });
          if (this->out())
          {
            Type value(Serialize<T>::convert(v));
            _serialize_switch<Type>(*this, name, value, ELLE_SFINAE_TRY());
          }
          else
          {
            Type value;
            _serialize_switch<Type>(*this, name, value, ELLE_SFINAE_TRY());
            v = Serialize<T>::convert(value);
          }
        }
      }
    }
    template <typename T>
    void
    Serializer::serialize(std::string const& name, boost::optional<T>& opt)
    {
      if (this->_out())
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            this->serialize(name, opt.get());
          });
      else
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            ELLE_ENFORCE(this->_enter(name));
            elle::SafeFinally leave([&] { this->_leave(name); });
            this->_serialize_anonymous(name, opt);
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
        _set_ptr(ptr, new T(static_cast<SerializerIn&>(s)));
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
        _set_ptr(ptr, new T);
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
          throw Error(elle::sprintf("unknown deserialization type: \"%s\"",
                                    type_name));
        _set_ptr(ptr, it->second(static_cast<SerializerIn&>(s)).release());
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
    Serializer::serialize(std::string const& name, std::unique_ptr<T>& opt,
                          bool anonymous)
    {
      if (this->_out())
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (anonymous)
              this->_serialize_anonymous(name, *opt);
            else
              this->serialize(name, *opt);
          });
      else
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (anonymous)
            {
              Details::_smart_virtual_switch<std::unique_ptr<T>, T>
                (*this, name, opt);
            }
            else if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              Details::_smart_virtual_switch<std::unique_ptr<T>, T>
                (*this, name, opt);
            }
          });
    }

    template <typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name,
                                     std::unique_ptr<T>& opt)
    {
      serialize(name, opt, true);
    }

    template <typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name,
                                     std::shared_ptr<T>& opt)
    {
      serialize(name, opt, true);
    }
    template <typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name,
                                     T*& opt)
    {
      serialize(name, opt, true);
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
    Serializer::serialize(std::string const& name, std::shared_ptr<T>& opt,
                          bool anonymous)
    {
      if (this->_out())
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (anonymous)
              this->_serialize_anonymous(name, *opt);
            else
              this->serialize(name, *opt);
          });
      else
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (anonymous)
            {
              Details::_smart_virtual_switch<std::shared_ptr<T>, T>
                (*this, name, opt);
            }
            else if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              Details::_smart_virtual_switch<std::shared_ptr<T>, T>
                (*this, name, opt);
            }
          });
    }

    template <typename T>
    typename std::conditional<true, void, typename Serialize<T>::Type>::type
    _serialize_ptr_switch(
      Serializer& s,
      std::string const& name,
      T& v,
      bool anonymous,
      ELLE_SFINAE_IF_POSSIBLE())
    {
      s.serialize_with_struct_serialize(name, v, anonymous);
    }

    template <typename T>
    void
    _serialize_ptr_switch(
      Serializer& s,
      std::string const& name,
      T& v,
      bool anonymous,
      ELLE_SFINAE_OTHERWISE())
    {
      s.serialize_ptr(name, v, anonymous);
    }
    template<typename T>
    void Serializer::serialize_ptr(std::string const& name, T* &opt, bool anonymous)
    {
      if (this->_out())
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (anonymous)
              this->_serialize_anonymous(name, *opt);
            else
              this->serialize(name, *opt);
          });
      else
      {
        opt = nullptr;
        this->_serialize_option(
          name,
          bool(opt),
          [&]
          {
            if (anonymous)
            {
              Details::_smart_virtual_switch<T*, T>
                (*this, name, opt);
            }
            else if (this->_enter(name))
            {
              elle::SafeFinally leave([&] { this->_leave(name); });
              Details::_smart_virtual_switch<T*, T>
                (*this, name, opt);
            }
          });
      }
    }

    template <typename T>
    void
    Serializer::serialize(std::string const& name, T* &opt, bool anonymous)
    {
      _serialize_ptr_switch(*this, name, opt, anonymous, ELLE_SFINAE_TRY());
      /*
      if (!anonymous)
      {
        if (this->_enter(name))
        {
          elle::SafeFinally leave([&] { this->_leave(name); });
          _serialize_ptr_switch(*this, name, opt, true, ELLE_SFINAE_TRY());
        }
      }
      else
        _serialize_ptr_switch(*this, name, opt, true, ELLE_SFINAE_TRY());
     */
    }

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::string const& name,
                           std::unordered_map<K, V, Rest...>& map)
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
      std::function<elle::Version ()> version,
      ELLE_SFINAE_IF_WORKS(object.serialize(ELLE_SFINAE_INSTANCE(Serializer),
                                            elle::Version())))
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
        auto version = _version_tag<T>(42);
        {
          ELLE_TRACE_SCOPE("%s: serialize version: %s", *this, version);
          auto guard = scoped_assignment(this->_versioned, false);
          this->serialize(".version", version);
        }
        _version_switch(*this, object,
                        [version] { return version; },
                        ELLE_SFINAE_TRY());
      }
      else
        _version_switch(*this, object,
                        [] { return _version_tag<T>(42); },
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

    template <typename T>
    void
    Serializer::_serialize_anonymous(std::string const& name, T& v)
    {
      _serialize_switch(*this, name, v, ELLE_SFINAE_TRY());
    }

    template <typename C>
    typename std::enable_if<
      is_unserializable_inplace<
        typename C::value_type>(),
        typename std::enable_if_exists<
          decltype(
            std::declval<C>().emplace(
              std::declval<elle::serialization::SerializerIn>())),
          void>::type>::type
    Serializer::_deserialize_in_array(std::string const& name,
                                      C& collection)
    {
      collection.emplace(static_cast<SerializerIn&>(*this));
    }

    template <typename C>
    typename std::enable_if<
      is_unserializable_inplace<
        typename C::value_type>(),
        typename std::enable_if_exists<
          decltype(
            std::declval<C>().emplace_back(
              std::declval<elle::serialization::SerializerIn>())),
          void>::type>::type
    Serializer::_deserialize_in_array(std::string const& name,
                                      C& collection)
    {
      collection.emplace_back(static_cast<SerializerIn&>(*this));
    }

    template <typename C>
    typename std::enable_if<
      !is_unserializable_inplace<typename C::value_type>(), void>::type
    Serializer::_deserialize_in_array(std::string const& name,
                                      C& collection)
    {
      collection.emplace_back();
      this->_serialize_anonymous(name, collection.back());
    }

    // Specific overload to catch std::vector subclasses (for das, namely).
    template <typename T, typename A>
    void
    Serializer::_serialize(std::string const& name,
                           std::vector<T, A>& collection)
    {
      this->_serialize<std::vector, T, A>(name, collection);
    }

    template <typename T, typename I>
    void
    Serializer::_serialize(
      std::string const& name,
      boost::multi_index::multi_index_container<T, I>& collection)
    {
      this->_serialize_collection(name, collection);
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
    Serializer::_serialize(std::string const& name, std::pair<T1, T2>& pair)
    {
      ELLE_LOG_COMPONENT("elle.serialization.Serializer");
      ELLE_TRACE_SCOPE("%s: serialize pair \"%s\"", *this, name);
      if (this->_out())
      {
        this->_serialize_array(
          name,
          2,
          [&] ()
          {
            if (this->_enter(name))
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("%s: serialize first member", *this);
              elle::SafeFinally leave([&] { this->_leave(name); });
              this->_serialize_anonymous(name, pair.first);
            }
            if (this->_enter(name))
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("%s: serialize second member", *this);
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
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("%s: deserialize first member", *this);
              this->_serialize_anonymous(name, pair.first);
            }
            else if (i == 1)
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              ELLE_DEBUG_SCOPE("%s: deserialize second member", *this);
              this->_serialize_anonymous(name, pair.second);
            }
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
      this->_serialize_anonymous("SERIALIZE ANONYMOUS", v);
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
            [] (SerializerIn& s)
            {
              ELLE_LOG_COMPONENT("elle.serialization.Serializer");
              // FIXME: factor reading version
              // FIXME: use that version
              if (s.versioned())
              {
                auto version = _version_tag<T>(42);
                {
                  ELLE_TRACE_SCOPE("serialize version: %s", version);
                  s.serialize(".version", version);
                }
              }
              return elle::make_unique<U>(s);
            };
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

    template <typename T> struct is_nullable
    {
      static const bool value = false;
    };
    template <typename T> struct is_nullable<T*> {static const bool value = true;};
    template <typename T> struct is_nullable<std::unique_ptr<T>> {static const bool value = true;};
    template <typename T> struct is_nullable<std::shared_ptr<T>> {static const bool value = true;};

    template <typename T>
    typename std::enable_if<
      !std::is_base_of<boost::optional_detail::optional_tag, T>::value
      && !is_nullable<T>::value,
      T>::type
    SerializerIn::deserialize(std::string const& name)
    {
      ELLE_ENFORCE(this->_enter(name));
      elle::SafeFinally leave([this, &name] { this->_leave(name); });
      return this->deserialize<T>();
    }

    template <typename T>
    typename std::enable_if<
      std::is_base_of<boost::optional_detail::optional_tag, T>::value
      || is_nullable<T>::value,
      T>::type
    SerializerIn::deserialize(std::string const& name)
    { // We cannot call _enter at this stage for optional types
      T res;
      this->serialize(name, res);
      return res;
    }

    template <typename T>
    T
    SerializerIn::deserialize()
    {
      return _deserialize<T>(*this);
    }

    template <typename T, typename Serialization>
    T
    deserialize(std::istream& input, bool version = true,
                boost::optional<Context const&> context = {})
    {
      typename Serialization::SerializerIn s(input, version);
      if (context)
        s.set_context(context.get());
      return s.template deserialize<T>();
    }

    template <typename T, typename Serialization>
    T
    deserialize(std::istream& input, std::string const& name,
                bool version = true)
    {
      typename Serialization::SerializerIn s(input, version);
      return s.template deserialize<T>(name);
    }

    template <typename T, typename Serialization>
    T
    deserialize(elle::Buffer const& input, bool version = true,
                boost::optional<Context const&> context = {})
    {
      elle::IOStream s(input.istreambuf());
      return deserialize<T, Serialization>(s, version, context);
    }

    template <typename T, typename Serialization>
    T
    deserialize(elle::Buffer const& input, std::string const& name,
                bool version = true)
    {
      elle::IOStream s(input.istreambuf());
      return deserialize<T, Serialization>(s, name, version);
    }

    // Prevent literal string from being converted to boolean and triggerring
    // the nameless overload.
    template <typename T, typename Serialization>
    T
    deserialize(elle::Buffer const& input, char const* name,
                bool version = true)
    {
      return deserialize<T, Serialization>(input, std::string(name), version);
    }

    template <typename T, typename Serialization>
    void
    serialize(T const& o, std::string const& name,
              std::ostream& output, bool version = true)
    {
      typename Serialization::SerializerOut s(output, version);
      s.serialize(name, o);
    }

    template <typename T, typename Serialization>
    void
    serialize(T const& o, std::ostream& output, bool version = true)
    {
      typename Serialization::SerializerOut s(output, version);
      s.serialize_forward(o);
    }

    template <typename T, typename Serialization>
    elle::Buffer
    serialize(T const& o, std::string const& name, bool version = true)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        serialize<T, Serialization>(o, name, s, version);
      }
      return res;
    }

    // Prevent literal string from being converted to boolean and triggerring
    // the nameless overload.
    template <typename T, typename Serialization>
    elle::Buffer
    serialize(T const& o, char const* name, bool version = true)
    {
      return serialize<T, Serialization>(o, std::string(name), version);
    }

    template <typename T, typename Serialization>
    elle::Buffer
    serialize(T const& o, bool version = true)
    {
      elle::Buffer res;
      {
        elle::IOStream s(res.ostreambuf());
        serialize<T, Serialization>(o, s, version);
      }
      return res;
    }

    template <typename T>
    constexpr
    typename std::enable_if_exists<typename T::serialization_tag, bool>::type
    _has_serialization_tag(int)
    {
      return true;
    }

    template <typename T>
    constexpr
    bool
    _has_serialization_tag(...)
    {
      return false;
    }

    template <typename T>
    constexpr
    bool
    has_serialization_tag()
    {
      return _has_serialization_tag<T>(42);
    }

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
      : public _forward_serialization_tag<T, has_serialization_tag<T>()>
    {};
  }
}

#endif

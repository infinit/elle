#ifndef ELLE_SERIALIZATION_SERIALIZER_HXX
# define ELLE_SERIALIZATION_SERIALIZER_HXX

# include <elle/Backtrace.hh>
# include <elle/finally.hh>
# include <elle/serialization/Error.hh>
# include <elle/serialization/SerializerIn.hh>
# include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    namespace _detail
    {
      template <typename T>
      typename std::enable_if<std::is_base_of<VirtuallySerializable, T>::value, void>::type
      _serialize_switch(
        Serializer& s,
        std::string const& name,
        T& obj,
        ELLE_SFINAE_IF_WORKS(obj.serialize(ELLE_SFINAE_INSTANCE(Serializer))))
      {
        s.serialize_object(name, obj);
        if (s.out())
        {
          auto const& map = Hierarchy<T>::_rmap();
          auto it = map.find(&typeid(obj));
          ELLE_ASSERT(it != map.end());
          auto type_name = it->second;
          s.serialize(T::virtually_serializable_key, type_name);
        }
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
      void
      _serialize_switch(Serializer& s,
                        std::string const& name,
                        T& v,
                        ELLE_SFINAE_OTHERWISE())
      {
        s.serialize_pod(name, v);
      }
    }

    template <typename T>
    void
    Serializer::serialize(std::string const& name, T& v)
    {
      if (this->_enter(name))
      {
        elle::SafeFinally leave([&] { this->_leave(name); });
        this->_serialize_anonymous(name, v);
      }
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
                                  // FIXME: no emplace in optional.
                                  T value;
                                  this->serialize(name, value);
                                  opt = value;
                                });
    }

    template <typename T>
    constexpr
    typename std::enable_if<sizeof(T(ELLE_SFINAE_INSTANCE(SerializerIn))) >= 0, bool>::type
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
        if (s._enter(name))
        {
          elle::SafeFinally leave([&] { s._leave(name); });
          ptr.reset(new T(static_cast<SerializerIn&>(s)));
        }
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
        s.serialize(name, *ptr);
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
        if (s._enter(name))
        {
          elle::SafeFinally leave([&] { s._leave(name); });
          auto const& map = Hierarchy<T>::_map();
          std::string type_name;
          s.serialize(T::virtually_serializable_key, type_name);
          auto it = map.find(type_name);
          if (it == map.end())
            throw Error(elle::sprintf("unable to deserialize type %s",
                                      type_name));
          ptr.reset(it->second(static_cast<SerializerIn&>(s)).release());
        }
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
            Details::_smart_virtual_switch<std::unique_ptr<T>, T>
              (*this, name, opt);

          });
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
            Details::_smart_virtual_switch<std::shared_ptr<T>, T>
              (*this, name, opt);
          });
    }

    template <typename K, typename V, typename ... Rest>
    void
    Serializer::_serialize(std::string const& name,
                           std::unordered_map<K, V, Rest...>& map)
    {
      if (this->_out())
      {
        this->_serialize_array(
          name,
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

    template <typename T>
    void
    Serializer::serialize_object(std::string const& name,
                                 T& object)
    {
      object.serialize(*this);
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
      _detail::_serialize_switch(*this, name, v, ELLE_SFINAE_TRY());
    }

    template <template <typename, typename> class C, typename T, typename A>
    void
    Serializer::_serialize(std::string const& name,
                           C<T, A>& collection)
    {
      if (!collection.empty())
      {
        this->_serialize_array(
          name,
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
          [&] ()
          {
            // FIXME: Use array.emplace_back(*this) if possible.
            collection.emplace_back();
            this->_serialize_anonymous(name, collection.back());
          });
      }
    }

    template <typename T1, typename T2>
    void
    Serializer::_serialize(std::string const& name, std::pair<T1, T2>& pair)
    {
      if (this->_out())
      {
        this->_serialize_array(
          name,
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
    class Hierarchy
    {
    public:
      template <typename U>
      class Register
      {
      public:
        Register(std::string const& name_ = "")
        {
          std::string name = name_.empty() ? demangle(typeid(U).name()) : name_;
          Hierarchy<T>::_map() [name] =
            [] (SerializerIn& s) { return elle::make_unique<U>(s); };
          Hierarchy<T>::_rmap()[&typeid(U)] = name;
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
      std::unordered_map<std::type_info const*, std::string>&
      _rmap()
      {
        static std::unordered_map<std::type_info const*, std::string> res;
        return res;
      }
    };
  }
}

#endif

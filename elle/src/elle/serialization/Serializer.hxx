#ifndef ELLE_SERIALIZATION_SERIALIZER_HXX
# define ELLE_SERIALIZATION_SERIALIZER_HXX

# include <elle/serialization/Error.hh>

namespace elle
{
  namespace serialization
  {
    namespace _detail
    {
      template <typename T>
      void
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
      this->_enter(name);
      this->_serialize_anonymous(name, v);
      this->_leave(name);
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
    void
    Serializer::serialize(std::string const& name, std::unique_ptr<T>& opt)
    {
      if (this->_out())
        this->_serialize_option(name,
                                bool(opt),
                                [&]
                                {
                                  this->serialize(name, *opt);
                                });
      else
        this->_serialize_option(name,
                                bool(opt),
                                [&]
                                {
                                  // FIXME: use in place constructor if available.
                                  opt.reset(new T);
                                  this->serialize(name, *opt);
                                });
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
              this->_serialize_anonymous(name, elt);
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
            this->_serialize_anonymous(name, pair.first);
            this->_serialize_anonymous(name, pair.second);
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
  }
}

#endif

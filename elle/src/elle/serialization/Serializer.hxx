#include <iostream>
#ifndef ELLE_SERIALIZATION_SERIALIZER_HXX
# define ELLE_SERIALIZATION_SERIALIZER_HXX

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

    template <typename T>
    void
    Serializer::_serialize(std::string const& name,
                           std::list<T>& array)
    {
      this->_serialize_array(
        name,
        [&] ()
        {
          std::cerr << "elt" << std::endl;
          // FIXME: Use array.emplace_back(*this) if possible.
          array.emplace_back();
          this->_serialize_anonymous(name, array.back());
          std::cerr << "/elt" << std::endl;
        });
    }
  }
}

#endif

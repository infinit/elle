#ifndef ELLE_FORMAT_JSON_ARRAY_HXX
# define ELLE_FORMAT_JSON_ARRAY_HXX

# include <boost/foreach.hpp>

# include "Object.hxx"
# include "Array.hh"


namespace elle { namespace format { namespace json {

    template<typename Container>
    Array::Array(Container const& container)
    {
      static_assert(
          detail::IsArray<Container>::value,
          "The constructor argument is not an iterable"
      );
      BOOST_FOREACH(auto val, container)
      {
        this->push_back(val);
      }
    }

    template<typename T> inline void Array::push_back(T const& value)
      {
        _value.push_back(Object::Factory::Construct(value).release());
      }

    // XXX should be "sequence" convertible, not just list
    template<typename T>
    Array::operator std::list<T>() const
    {
      std::list<T> result;
      for (Object* obj : _value)
      {
        result.push_back(obj->as<T>());
      }
      return result;
    }

}}}

#endif

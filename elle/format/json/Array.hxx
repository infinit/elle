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

}}}

#endif

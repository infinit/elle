#ifndef  ELLE_FORMAT_JSON_OBJECT_HXX
# define ELLE_FORMAT_JSON_OBJECT_HXX

# include <typeinfo>

# include "_detail.hh"

# include "Float.hh"
# include "String.hh"
# include "Integer.hh"
# include "Bool.hh"
# include "Null.hh"
# include "Dictionary.hh"
# include "Array.hh"
# include "Null.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      template <typename T>
      void Object::Load(T& out) const
      {
        static_assert(
            !std::is_base_of<T, Object>::value,
            "Cannot load into a json object"
        );
        typedef typename detail::SelectJSONType<T>::type SelfType;
        out = dynamic_cast<SelfType const&>(*this);
      }

      template <typename T>
      T Object::as() const
      {
        T val;
        this->Load(val);
        return val;
      }

      template <typename T>
      bool Object::TryLoad(T& out) const
      {
        typedef typename detail::SelectJSONType<T>::type SelfType;
        if (auto ptr = dynamic_cast<SelfType const*>(this))
          {
            out = *ptr;
            return true;
          }
        return false;
      }


      template <typename T> typename std::enable_if<
            !std::is_base_of<T, Object>::value
          , bool
      >::type Object::operator ==(T const& other) const
      {
        typedef typename detail::SelectJSONType<T>::type SelfType;
        if (auto self = dynamic_cast<SelfType const*>(this))
          return *self == SelfType(other);
        return false;
      }

    }
  }
} // !namespace elle::format::json

#endif
